

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

#include <deque>
#include <iterator>
#include <unordered_map>
#include <unordered_set>

#include "CallGraph.h"
#include "DoubleLocking.h"


using namespace callgraphs;

using namespace llvm;

AnalysisKey DoubleLocking::Key;

std::unordered_map<llvm::Instruction*,ErrorKind>
	DoubleLocking::run(llvm::Module &M, llvm::ModuleAnalysisManager &AM) {
  auto *mainFunction = M.getFunction("main");
  assert(mainFunction && "Unable to find main function");
  auto &callgraph = AM.getResult<WeightedCallGraph>(M);

  // The entry point to your analysis / pass is here.

  //return false;
}

/*
void
DoubleLocking::logError(llvm::Instruction *location, ErrorKind kind) {
  errors[location] = kind;
}
*/

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

