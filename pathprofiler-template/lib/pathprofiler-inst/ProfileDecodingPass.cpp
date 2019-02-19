

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Support/raw_ostream.h"

#include <unordered_map>

#include "ProfileDecodingPass.h"
#include "InnermostLoops.h"


using namespace llvm;
using namespace pathprofiling;


namespace pathprofiling {
char ProfileDecodingPass::ID = 0;
}


void
printPath(std::vector<llvm::BasicBlock*> &blocks) {
  unsigned line = 0;
  llvm::StringRef file;
  for (auto *bb : blocks) {
    for (auto &instruction : *bb) {
      DILocation *loc = instruction.getDebugLoc();
      if (loc && (loc->getLine() != line || loc->getFilename() != file)) {
        line = loc->getLine();
        file = loc->getFilename();
        outs() << "File " << file.str() << " line " << line << "\n";
      }
    }
  }
}


struct Path {
  Loop *loop;
  uint64_t id;
  uint64_t count;

  bool operator<(const Path &other) {
    return count < other.count;
  }
};


bool
ProfileDecodingPass::runOnModule(Module &module) {

  //

  std::vector<std::vector<llvm::BasicBlock*>> bbSequences;

  //

  outs() << "Top " << bbSequences.size() << " Paths\n"
         << "=========================================\n";
  for (size_t i = 0, e = bbSequences.size(); i < e; ++i) {
    outs() << "Path, occurrences: " << paths[i].count << "\n";
    printPath(bbSequences[i]);
    outs() << "\n";
  }

  return false;
}


std::vector<llvm::BasicBlock*>
ProfileDecodingPass::decode(llvm::Loop *loop, uint64_t pathID) {
  std::vector<llvm::BasicBlock*> sequence;

  //

  return sequence;
}

