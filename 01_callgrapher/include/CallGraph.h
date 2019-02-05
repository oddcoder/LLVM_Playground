
#ifndef CALLGRAPH_H
#define CALLGRAPH_H

#include "llvm/IR/CallSite.h"
#include "llvm/IR/PassManager.h"

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

struct WeightedCallGraphInfo {
	std::map<std::string, function *> function_list;
	void print(llvm::raw_ostream &out) const;
	void Analyse(llvm::Module &m, llvm::ModuleAnalysisManager &MAM);
private:
	void AnalyseCallSite(llvm::CallSite &cs, llvm::ModuleAnalysisManager &MAM);
	void Analyse(llvm::Function &f, llvm::ModuleAnalysisManager &MAM);
	void AnalyseDirectCallSite(llvm::CallSite &cs, llvm::ModuleAnalysisManager &MAM);
	void AnalyseIndirectCallSite(llvm::CallSite &cs, llvm::ModuleAnalysisManager &MAM);

};

struct WeightedCallGraph: public llvm::AnalysisInfoMixin<WeightedCallGraph> {
	friend AnalysisInfoMixin<WeightedCallGraph>;
	static llvm::AnalysisKey Key;

	using Result = WeightedCallGraphInfo;
	Result run(llvm::Module &M, llvm::ModuleAnalysisManager &AM);
};

struct WeightedCallGraphPrinter:public
			llvm::PassInfoMixin<WeightedCallGraphPrinter> {
	//static llvm::AnalysisKey Key;

	llvm::raw_ostream &OS;
	using Result = llvm::PreservedAnalyses;
	explicit WeightedCallGraphPrinter(llvm::raw_ostream &OS) : OS(OS) {}
	Result run(llvm::Module &M, llvm::ModuleAnalysisManager &AM);
};
}

#endif
