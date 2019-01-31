#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/CallSite.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
	struct Hello : public FunctionPass {
		static char ID;
		Hello(): FunctionPass(ID) {}

		bool runOnFunction(Function &F) override {
			errs() << "Hello: ";
			errs().write_escaped(F.getName()) << '\n';
			return false;
		}
	}; // end of struct Hello
} // end of the anonymous namespace

char Hello::ID = 0;
static RegisterPass<Hello> X("hello", "Hello World Pass");

namespace{
	struct Hello2 : public FunctionPass {
		static char ID;
		Hello2() : FunctionPass(ID) {}
		bool runOnFunction(Function &F) override {
			unsigned int basicBlockCount = 0;
			unsigned int instCount = 0;
			for (BasicBlock &bb : F) {
				basicBlockCount++;
				for (Instruction &i : bb)
					instCount++;
			}
			errs() << "Hello2: ";
			errs().write_escaped(F.getName());
			errs() << " Basic blocks " << basicBlockCount
				<< " Instruction " << instCount << "\n";
			return false;
		}
	};
}

char Hello2::ID = 0;
static RegisterPass<Hello2> X2("hello2", "Hello World Pass");

namespace{
	struct Hello3: public FunctionPass {
		static char ID;
		Hello3() : FunctionPass(ID) {}
		bool runOnFunction(Function &F) override {
			for (BasicBlock &bb: F){
				for (Instruction &i: bb){
					CallSite cs(&i);
					if (!cs.getInstruction())
						continue;
					Value *called = cs.getCalledValue()->stripPointerCasts();
					if (Function *f = dyn_cast<Function>(called))
						errs() <<"Direct call to "
							<< f->getName()
							<<" From "
							<< F.getName()
							<< "\n";
				}
				return false;
			}
		}
	};
}

char Hello3::ID = 0;
static RegisterPass<Hello3> X3("hello3", "Hello World Pass");


