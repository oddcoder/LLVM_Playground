
#include "CallGraph.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/Analysis/MemoryDependenceAnalysis.h"
#include "llvm/Analysis/AliasSetTracker.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"

#include <iterator>

using namespace llvm;
using namespace callgraphs;


/*RegisterPass<LegacyWeightedCallGraphPass> X("weightedcg",
                                "construct a weighted call graph of a module");
*/
AnalysisKey WeightedCallGraph::Key;
AnalysisKey WeightedCallGraphPrinter::Key;
void WeightedCallGraphInfo::AnalyseCallSite(CallSite &cs) {
	if (cs.isIndirectCall())
		AnalyseIndirectCallSite(cs);
	else
		AnalyseDirectCallSite(cs);
}
void WeightedCallGraphInfo::AnalyseIndirectCallSite(CallSite &cs) {
	Value *v = cs.getCalledValue();
	Function *caller = cs.getCaller();
	//AliasAnalysis &AA = getAnalysis<AAResultsWrapperPass>(/**caller*/).getAAResults();
	Module *m = caller->getParent();
	for (Function &f: *m) {
	}
	/*AliasSetTracker tr(AA);
	for (BasicBlock &bb: *caller) {
		for(Instruction &i: bb) {
			tr.add(&i);
		}
	}
	AliasSet &set = tr.getAliasSetFor(v);*/
}

void WeightedCallGraphInfo::AnalyseDirectCallSite(CallSite &cs) {
	Function *caller, *callee;
	struct called c;
	caller = cs.getCaller();
	callee = cs.getCalledFunction();
	if (callee->isIntrinsic())
		return;
	if (this->function_list.count(callee->getName()) == 0)
		Analyse(*callee);
	c.f.push_back(this->function_list[callee->getName()]);
	c.f.back()->weight++;
	const DebugLoc debugloc = cs.getInstruction()->getDebugLoc();
	c.line = debugloc.getLine();
	c.file = cs.getInstruction()->getModule()->getSourceFileName().c_str();
	this->function_list[caller->getName()]->call_list.push_back(c);
}


// At this point we assume that all we know is that we
// never encounter this function before.
// no more assumptions
void WeightedCallGraphInfo::Analyse(Function &f) {
	struct function *fun;
	if (f.isIntrinsic())
		return;
	fun = new function;
	fun->name = f.getName().data();
	fun->weight=0;
	this->function_list.insert({f.getName().data(), fun});
	for (BasicBlock &bb : f) {
		for (Instruction &i : bb) {
			CallSite cs(&i);
			if (!cs.getInstruction())
				continue;
			AnalyseCallSite(cs);

		}
	}
}


// For an analysis pass, runOnModule should perform the actual analysis and
// compute the call graph. The actual output, however, is produced separately
// by the print function.
void WeightedCallGraphInfo::Analyse(Module &m) {

  for (Function &fun : m){
    if (this->function_list.count(fun.getName()) == 0)
	    Analyse(fun);
  }
}


void
WeightedCallGraphInfo::print(raw_ostream &out) const {
  out << "digraph {\n  node [shape=record];\n";

  // Print out all function nodes
  for (auto const &kv: this->function_list) {
    struct function *func = kv.second;
    out << "  \"" << func->name
        << "\"[label=\"{" << func->name
        << "|Weight : " << func->weight;

    unsigned lineID = 0;
    for (auto const &callsite: func->call_list) {
      out << "|<l" << lineID << ">"
          << callsite.file << " : "
          << callsite.line;
      ++lineID;
    }
    out << "}\"];\n";
  }

  // Print the edges between them
  for (auto const &kv: this->function_list) {
    struct function *func = kv.second;
    unsigned lineID = 0;
    //for (auto const& function: this->function_list) {
    for (auto const &callsite: func->call_list) {
      for (auto const callee: callsite.f) {
	out << "  " << func->name
            << ":l" << lineID
            << " -> " << callee->name<< ";\n";
	++lineID;
      }
    }
  }

  out << "}\n";
}

struct WeightedCallGraphInfo WeightedCallGraph::run(Module &M,
		ModuleAnalysisManager &AM) {
	struct WeightedCallGraphInfo Result;
	Result.Analyse(M);
	return Result;
}

PreservedAnalyses WeightedCallGraphPrinter::run(Module &M,
						ModuleAnalysisManager &AM) {
	AM.getResult<WeightedCallGraph>(M).print(OS);
	return PreservedAnalyses::all();
}



void registerCallbacks(PassBuilder &PB) {
  PB.registerAnalysisRegistrationCallback(
      [&](ModuleAnalysisManager &MAM) {
	MAM.registerPass([&] {return WeightedCallGraph();});
	MAM.registerPass([&] {return WeightedCallGraphPrinter(outs());});
	return true;
      });
}


extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
	return {
		LLVM_PLUGIN_API_VERSION,
		"WeightedCallGraphInfo",
		"v0.1",
		registerCallbacks
	};
}

