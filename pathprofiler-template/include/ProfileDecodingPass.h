

#ifndef PROFILEDECODINGPASS_H
#define PROFILEDECODINGPASS_H


#include "llvm/Analysis/LoopInfo.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"

#include "PathEncodingPass.h"

namespace pathprofiling {


struct ProfileDecodingPass : public llvm::ModulePass {

  static char ID;

  const llvm::StringRef filename;

  const size_t numberToReturn;

  ProfileDecodingPass(llvm::StringRef filename, size_t numberToReturn)
    : llvm::ModulePass(ID),
      filename(filename),
      numberToReturn(numberToReturn)
      { }

  virtual void getAnalysisUsage(llvm::AnalysisUsage &au) const override {
    au.addRequired<llvm::LoopInfoWrapperPass>();
    au.addRequired<PathEncodingPass>();
  }

  virtual bool runOnModule(llvm::Module &m) override;

  std::vector<llvm::BasicBlock*>
  decode(llvm::Loop *loop, uint64_t pathID);
};


}


#endif

