

#ifndef PATHENCODINGPASS_H
#define PATHENCODINGPASS_H


#include "llvm/Analysis/LoopInfo.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"


namespace pathprofiling {


struct PathEncodingPass : public llvm::ModulePass {

  static char ID;

  // TODO: You will probably want to add things to this class.

  PathEncodingPass()
    : llvm::ModulePass(ID)
      { }

  virtual void getAnalysisUsage(llvm::AnalysisUsage &au) const override {
    au.addRequired<llvm::LoopInfoWrapperPass>();
    au.setPreservesAll();
  }

  virtual bool runOnModule(llvm::Module &m) override;

  void encode(llvm::Loop *loop);
};


}


#endif

