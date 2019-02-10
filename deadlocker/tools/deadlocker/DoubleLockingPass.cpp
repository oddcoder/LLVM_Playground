

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
#include "DoubleLockingPass.h"


using namespace callgraphs;


char DoubleLockingPass::ID = 0;


bool
DoubleLockingPass::runOnModule(llvm::Module &m) {
  auto *mainFunction = m.getFunction("main");
  assert(mainFunction && "Unable to find main function");
  cgPass = &getAnalysis<CallGraphPass>();

  // The entry point to your analysis / pass is here.

  return false;
}


void
DoubleLockingPass::logError(llvm::Instruction *location, ErrorKind kind) {
  errors[location] = kind;
}


static void
printLineNumber(llvm::raw_ostream &out, llvm::Instruction *inst) {
  if (llvm::MDNode *n = inst->getMetadata("dbg")) {
    llvm::DILocation loc(n);
    out << "At " << loc.getFilename()
        << " line " << loc.getLineNumber()
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
void
DoubleLockingPass::print(llvm::raw_ostream &out, const llvm::Module *m) const {
  for (auto &errorPair : errors) {
    llvm::Instruction *mutexOperation;
    ErrorKind errorKind;
    std::tie(mutexOperation, errorKind) = errorPair;

    out.changeColor(llvm::raw_ostream::Colors::RED);
    printLineNumber(out, mutexOperation);

    auto *called = getCalledFunction(mutexOperation);
    out.changeColor(llvm::raw_ostream::Colors::YELLOW);
    out << "Detected " << getErrorKindString(errorKind)
        << " error in call to " << called->getName() << "\n";
  }

  if (errors.empty()) {
    out.changeColor(llvm::raw_ostream::Colors::GREEN);
    out << "No errors detected\n";
  }
  out.resetColor();
}

