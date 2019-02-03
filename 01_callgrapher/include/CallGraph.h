
#ifndef CALLGRAPH_H
#define CALLGRAPH_H

#include "llvm/IR/Function.h"
#include "llvm/IR/CallSite.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Analysis/MemoryDependenceAnalysis.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace callgraphs {

struct called;
struct function;


struct function {
	const char *name;
	int weight;
	std::vector<struct called> call_list;
};

struct called {
	const char *file;
	int line;
	std::vector<struct function *> f;
};

struct WeightedCallGraphPass : public llvm::ModulePass {
	static char ID;
	
	std::map<std::string, function *> function_list;

	WeightedCallGraphPass() : ModulePass(ID)
	{ }
  
  virtual ~WeightedCallGraphPass() { }

  virtual void getAnalysisUsage(llvm::AnalysisUsage &au) const override {
    au.setPreservesAll();
    au.addRequired<llvm::AAResultsWrapperPass>();
  }

  virtual void print(llvm::raw_ostream &out,
                     const llvm::Module *m) const override;
  void AnalyseCallSite(llvm::CallSite &cs);
  void Analyse(llvm::Function &f);
  void AnalyseDirectCallSite(llvm::CallSite &cs);
  void AnalyseIndirectCallSite(llvm::CallSite &cs);
  virtual bool runOnModule(llvm::Module &m) override;
};


}

#endif
