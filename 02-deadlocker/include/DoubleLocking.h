
#ifndef DOUBLELOCKINGPASS_H
#define DOUBLELOCKINGPASS_H


#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "numeric"
#include "CallGraph.h"


enum class ErrorKind {
  DOUBLE_LOCK,
  DOUBLE_UNLOCK
};

using Context  = std::array<llvm::Instruction*,2>;


// Approach for hash combining taken from boost
template <class T>
inline std::size_t
hash_combine(std::size_t seed, T const &v) noexcept {
  return seed ^ std::hash<T>()(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}


struct ContextHash {
  size_t
  operator()(Context const &a) const noexcept {
    return std::accumulate(std::begin(a), std::end(a), size_t(0),
      hash_combine<Context::value_type>);
  }
};


struct DoubleLocking : public llvm::AnalysisInfoMixin<DoubleLocking> {
	friend AnalysisInfoMixin<DoubleLocking>;
	static llvm::AnalysisKey Key;
	std::unordered_map<llvm::Instruction*,ErrorKind> errors;
	using Result = std::unordered_map<llvm::Instruction*,ErrorKind>;
	Result run(llvm::Module &M, llvm::ModuleAnalysisManager &AM);
};
/*
  virtual void getAnalysisUsage(llvm::AnalysisUsage &au) const override {
    au.setPreservesAll();
    au.addRequired<llvm::DataLayoutPass>();
    au.addRequired<callgraphs::CallGraphPass>();
  }

  virtual bool runOnModule(llvm::Module &m) override;

  void print(llvm::raw_ostream &out, const llvm::Module *m) const;

  void logError(llvm::Instruction*, ErrorKind kind);
};
*/
struct DoubleLockingPrinter:public
			    llvm::PassInfoMixin<DoubleLockingPrinter> {

        llvm::raw_ostream &OS;
        using Result = llvm::PreservedAnalyses;
        explicit DoubleLockingPrinter(llvm::raw_ostream &OS) : OS(OS) {}
        Result run(llvm::Module &M, llvm::ModuleAnalysisManager &AM);
};

#endif

