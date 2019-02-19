

#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/GraphWriter.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include <unordered_map>

#include "PathProfilingPass.h"
#include "InnermostLoops.h"


using namespace llvm;
using namespace pathprofiling;


namespace pathprofiling {
char PathProfilingPass::ID = 0;
}


bool
PathProfilingPass::runOnModule(Module &module) {

  //

  return true;
}


void
PathProfilingPass::instrument(Module &module, Value *counter,
                              Loop *loop, uint64_t loopID) {

  //

}

