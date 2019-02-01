
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

struct called;
struct function;


struct function {
	char *name;
	int weight;
	std::vector<struct called> call_list;
};

struct called {
	char *file;
	int line;
	struct function *f;
};

struct WeightedCallGraphPass : public llvm::ModulePass {
	static char ID;
	
	std::vector<function> function_list;

	WeightedCallGraphPass() : ModulePass(ID)
	{ }
  
  virtual ~WeightedCallGraphPass() { }

  virtual void getAnalysisUsage(llvm::AnalysisUsage &au) const override {
    au.setPreservesAll();
    //au.addRequired<llvm::DataLayout>();
  }

  virtual void print(llvm::raw_ostream &out,
                     const llvm::Module *m) const override;

  virtual bool runOnModule(llvm::Module &m) override;
};


}

#endif
