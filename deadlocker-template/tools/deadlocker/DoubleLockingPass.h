
#ifndef DOUBLELOCKINGPASS_H
#define DOUBLELOCKINGPASS_H


#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"

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


struct DoubleLockingPass : public llvm::ModulePass {

  static char ID;

  callgraphs::CallGraphPass *cgPass;

  std::unordered_map<llvm::Instruction*,ErrorKind> errors;

  DoubleLockingPass()
    : ModulePass{ID},
      cgPass{nullptr}
      { }

  virtual void getAnalysisUsage(llvm::AnalysisUsage &au) const override {
    au.setPreservesAll();
    au.addRequired<llvm::DataLayoutPass>();
    au.addRequired<callgraphs::CallGraphPass>();
  }

  virtual bool runOnModule(llvm::Module &m) override;

  void print(llvm::raw_ostream &out, const llvm::Module *m) const;

  void logError(llvm::Instruction*, ErrorKind kind);
};


#endif

