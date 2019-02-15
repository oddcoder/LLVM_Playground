

#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/CallSite.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Pass.h"
#include "llvm/Analysis/AliasAnalysis.h"

#include <deque>
#include <iterator>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <vector>

#include "DoubleLocking.h"


using namespace llvm;

enum MutexState {
	Bottom = 0,
	Unlocked = 1,
	Locked = 2,
	Top = 3,
};

struct AbstractState {
	Value *value;
	MutexState before;
	MutexState after;

};

AnalysisKey DoubleLocking::Key;

using AbstractStateBB = std::unordered_map<Value *,AbstractState>;
using AbstractStateMap = std::unordered_map<BasicBlock *, AbstractStateBB *>;
void meet(AbstractStateBB *dst, AbstractStateBB *src) {
	for (auto vs: *src) {
		if (dst->count(vs.first) == 0) {
			AbstractState s;
			s.value = vs.first;
			s.before = vs.second.after;
			s.after = Bottom;
			(*dst)[vs.first] = s;
		} else {
			(*dst)[vs.first].before =
				static_cast<MutexState>(
			 		(*dst)[vs.first].before |
					vs.second.after
				);
		}
	
	}
}

bool same_before(AbstractStateBB *a, AbstractStateBB *b) {
	if(a->size() != b->size())
		return false;
	for (auto vs : *a) {
		Value *v = vs.first;
		if (b->count(v) == 0)
			return false;
		if ((*b)[v].before != (*a)[v].before)
			return false;

	}
	return true;
}
bool same_after(AbstractStateBB *a, AbstractStateBB *b) {
	if(a->size() != b->size())
		return false;
	for (auto vs : *a) {
		Value *v = vs.first;
		if (b->count(v) == 0)
			return false;
		if ((*b)[v].after != (*a)[v].after)
			return false;

	}
	return true;
}
struct ArgumentMetadata {
	std::unordered_map<Value *, Value *> subs;
	AbstractStateBB *callerState=NULL;
	AbstractStateBB *retState=NULL;
};
class ArgumentTracer : public std::vector<ArgumentMetadata> {
	public:
	void pushCallSite(CallSite *cs, AbstractStateBB *callerstate);
	Value *resolve (Value *v, AAResults &AA);
	AbstractStateBB *getCallerState();
	void ret(AbstractStateBB *retState);
	AbstractStateBB *pop();
};

AbstractStateBB *ArgumentTracer::getCallerState() {
	return this->back().callerState;
}
AbstractStateBB *ArgumentTracer::pop() {
	ArgumentMetadata M = this->back();
	this->pop_back();
	return M.retState;
}

void ArgumentTracer::ret(AbstractStateBB *retState) {
	this->back().retState = retState;
}


void ArgumentTracer::pushCallSite(CallSite *cs, AbstractStateBB *callerState) {
	auto fun = cs->getCalledFunction();
	ArgumentMetadata M;
	Function::arg_iterator AI, AE;
	int i;
	for (AI = fun->arg_begin(), AE = fun->arg_end(), i = 0; AI != AE; ++AI, ++i) {
		Value *v = &*AI;
		M.subs[v] = cs->getArgument(i);
		//use push back
	}
	M.callerState = callerState;
	this->push_back(M);
}
Value *ArgumentTracer::resolve (Value *v, AAResults &AA) {
	Value *ret = v;
	ArgumentTracer::iterator i = this->end();
	while (i != this->begin()) {
		--i;
		ArgumentMetadata lookup = *i;
		if (lookup.subs.count(ret) != 0) {
			ret = lookup.subs[ret];
			continue;
		}
		// check that none of the argument alias with a value we have;
		for (auto kv: lookup.subs) {
			if(AA.alias(v, kv.first) == AliasResult::MustAlias) {
				ret = kv.second;
				continue;
			}
		}
		break;
	}
	return ret;

}
class DataFlow {
	int maxDepth;
	ArgumentTracer arguments;
	AbstractStateMap state;
	FunctionAnalysisManager *FAM;
	void transfer(AbstractStateBB *state, Instruction *i);
	ErrorKind initialize_mutex(AbstractStateBB *state, Value *v);
	ErrorKind lock_mutex(AbstractStateBB *state, Value *v);
	ErrorKind unlock_mutex(AbstractStateBB *state, Value *v);
	
	

	public:
	std::unordered_map<llvm::Instruction*,ErrorKind> error;
	DataFlow(FunctionAnalysisManager *F ,int i) {
		maxDepth = i;
		FAM = F;
	}
	void computeMutexState(Function *f);

};

ErrorKind DataFlow::initialize_mutex(AbstractStateBB *state, Value *v) {
	if (state->count(v) == 0) {
		AbstractState A;
		A.value=v;
		A.before= MutexState::Bottom;
		A.after = MutexState::Unlocked;
		(*state)[v] = A;
	} else {
		(*state)[v].after = MutexState::Unlocked;
	}
	return ErrorKind::NO_ERROR;
}
ErrorKind DataFlow::lock_mutex(AbstractStateBB *state, Value *v) {
	ErrorKind e = ErrorKind::NO_ERROR;
	if (state->count(v) == 0) {
		e = ErrorKind::UNINIT_LOCK;
		AbstractState A;
		A.value=v;
		A.before= MutexState::Bottom;
		A.after = MutexState::Locked;
		(*state)[v] = A;

	} else {
		if ((*state)[v].after != MutexState::Unlocked) {
			e = ErrorKind::DOUBLE_LOCK;
		}
		(*state)[v].after = MutexState::Locked;
	}
	return e;
}
ErrorKind DataFlow::unlock_mutex(AbstractStateBB *state, Value *v) {
	ErrorKind e = ErrorKind::NO_ERROR;

	if (state->count(v) == 0) {
		e = ErrorKind::UNINIT_UNLOCK;
		AbstractState A;
		A.value=v;
		A.before= MutexState::Bottom;
		A.after = MutexState::Unlocked;
		(*state)[v] = A;

	} else {
		if ((*state)[v].after != MutexState::Locked) {
			e = ErrorKind::DOUBLE_UNLOCK;
		}
		(*state)[v].after = MutexState::Unlocked;
	}
	return e;
}

void DataFlow::transfer(AbstractStateBB *state, Instruction *i) {
	CallSite cs(i);
	if (!cs.getInstruction())
		return;
	auto *fun = cs.getCalledFunction();
	if (!fun) {
		return;
	}
	ErrorKind e = ErrorKind::NO_ERROR;
	  AAResults &AA = this->FAM->getResult<AAManager>(*fun);

	if (fun->getName() == "pthread_mutex_init") {
		Value *v = arguments.resolve(cs.getArgument(0), AA);
		e = initialize_mutex(state, v);
	} else if(fun->getName() == "pthread_mutex_lock") {
		Value *v = arguments.resolve(cs.getArgument(0), AA);
		e = lock_mutex(state, v);
	} else if (fun->getName() == "pthread_mutex_unlock") {
		Value *v = arguments.resolve(cs.getArgument(0), AA);
		e = unlock_mutex(state, v);
	} else if (!fun->isIntrinsic()) {
		this->arguments.pushCallSite(&cs, state);
		computeMutexState(fun);
		AbstractStateBB *ret = this->arguments.pop();
		if(ret) for (auto kv: *state) {
			if (ret->count(kv.first) != 0) {
				(*state)[kv.first].after =
					(*ret)[kv.first].after;
			}
		}
	}
	if (e != ErrorKind::NO_ERROR) {
		this->error[i] = e;
	}
}

void DataFlow::computeMutexState(Function *f) {
	std::queue<BasicBlock *> workList;
	if(this->arguments.size() > this->maxDepth)
		return;
	for (auto &bb : *f) {
		this->state[&bb] = new AbstractStateBB();
		workList.push(&bb);
	}
	
	while(!workList.empty()) {
		auto bb = workList.front();
		workList.pop();
		auto oldState = this->state[bb];

		AbstractStateBB *newState = new AbstractStateBB();
		if ( bb == &f->getEntryBlock() && this->arguments.size() != 0) {
			*newState = *arguments.getCallerState();
		}
		for (auto p : predecessors(bb)) {
			meet(newState,this->state[p]);
		}
		if(same_before(oldState, newState) && !newState->empty()) {
			delete newState;
			continue;
		}

		for (auto &i : *bb) {
			transfer(newState, &i);
		}
		if(same_after(oldState, newState)) {
			delete newState;
			continue;
		} else {
			this->state[bb] = newState;
			delete oldState;
		}
		for (auto s : llvm::successors(bb)) {
			workList.push(s);
		}
	}
	AbstractStateBB *retState = new AbstractStateBB();
	
	if (this->arguments.size() != 0) {
		for (auto &bb: *f) {
			Instruction *i = bb.getTerminator();
			if (ReturnInst *ri = dyn_cast<ReturnInst>(i)) {
				meet(retState, this->state[&bb]);
			}
		}
		for(auto kv: *retState){
			(*retState)[kv.first].after = (*retState)[kv.first].before;
		}
		arguments.ret(retState);
	}
	return;
}

std::unordered_map<llvm::Instruction*,ErrorKind>
	DoubleLocking::run(llvm::Module &M, llvm::ModuleAnalysisManager &AM) {
  auto *mainFunction = M.getFunction("main");
  assert(mainFunction && "Unable to find main function");
  FunctionAnalysisManager &FAM =
      AM.getResult<FunctionAnalysisManagerModuleProxy>(M).getManager();
    
  DataFlow d = DataFlow(&FAM, 20);
  d.computeMutexState(mainFunction);
  return d.error;

  // The entry point to your analysis / pass is here.

  //return false;
}

static void
printLineNumber(llvm::raw_ostream &out, llvm::Instruction *inst) {
  if ( const DebugLoc debugloc = inst->getDebugLoc()) {
    out << "At " << inst->getModule()->getSourceFileName().c_str()
        << " line " << debugloc.getLine()
        << ":\n";
  } else {
    out << "At an unknown location:\n";
  }
}


static const char *
getErrorKindString(ErrorKind kind) {
  switch(kind) {
    case ErrorKind::DOUBLE_LOCK:   return "double locking";
    case ErrorKind::DOUBLE_UNLOCK: return "double unlocking";
    case ErrorKind::UNINIT_LOCK:   return "locking without initialization";
    case ErrorKind::UNINIT_UNLOCK: return "unlocking without initialization";


  }
}


// Output for a pure analysis pass should happen in the print method.
// It is called automatically after the analysis pass has finished collecting
// its information.
llvm::PreservedAnalyses
DoubleLockingPrinter::run(llvm::Module &M, llvm::ModuleAnalysisManager &AM) {
	auto &errors = AM.getResult<DoubleLocking>(M);
	for (auto &errorPair : errors) {
		llvm::Instruction *mutexOperation;
		ErrorKind errorKind;
		std::tie(mutexOperation, errorKind) = errorPair;

		OS.changeColor(llvm::raw_ostream::Colors::RED);
		printLineNumber(OS, mutexOperation);

		auto *called = mutexOperation->getFunction();
		OS.changeColor(llvm::raw_ostream::Colors::YELLOW);
		OS << "Detected " << getErrorKindString(errorKind)
			<< " error in call to " << called->getName() << "\n";
	}

	if (errors.empty()) {
		OS.changeColor(llvm::raw_ostream::Colors::GREEN);
		OS << "No errors detected\n";
	}
	OS.resetColor();
}

