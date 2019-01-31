
#ifndef CALLGRAPH_H
#define CALLGRAPH_H

#include "llvm/IR/CallSite.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace callgraphs {


// You may add anything you wish to this file.


struct WeightedCallGraphPass : public llvm::ModulePass {

  static char ID;

  // NOTE: Feel free to modify this class as you see fit.

  WeightedCallGraphPass()
    : ModulePass(ID)
      { }
  
  virtual ~WeightedCallGraphPass() { }

  virtual void getAnalysisUsage(llvm::AnalysisUsage &au) const override {
    au.setPreservesAll();
    au.addRequired<llvm::DataLayoutPass>();
  }

  virtual void print(llvm::raw_ostream &out,
                     const llvm::Module *m) const override;

  virtual bool runOnModule(llvm::Module &m) override;
};


}

#endif
