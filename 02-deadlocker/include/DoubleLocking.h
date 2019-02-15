
#ifndef DOUBLELOCKINGPASS_H
#define DOUBLELOCKINGPASS_H


#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "numeric"
#include <unordered_map>
#include <unordered_set>

enum class ErrorKind {
  NO_ERROR,
  DOUBLE_LOCK,
  DOUBLE_UNLOCK,
  UNINIT_LOCK,
  UNINIT_UNLOCK,
};

struct DoubleLocking : public llvm::AnalysisInfoMixin<DoubleLocking> {
	friend AnalysisInfoMixin<DoubleLocking>;
	static llvm::AnalysisKey Key;
	std::unordered_map<llvm::Instruction*,ErrorKind> errors;
	using Result = std::unordered_map<llvm::Instruction*,ErrorKind>;
	Result run(llvm::Module &M, llvm::ModuleAnalysisManager &AM);
};

struct DoubleLockingPrinter:public
			    llvm::PassInfoMixin<DoubleLockingPrinter> {

        llvm::raw_ostream &OS;
        using Result = llvm::PreservedAnalyses;
        explicit DoubleLockingPrinter(llvm::raw_ostream &OS) : OS(OS) {}
        Result run(llvm::Module &M, llvm::ModuleAnalysisManager &AM);
};

#endif

