
#include "CallGraph.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/DebugLoc.h"

#include <iterator>

using namespace llvm;
using namespace callgraphs;


char WeightedCallGraphPass::ID = 0;

RegisterPass<WeightedCallGraphPass> X("weightedcg",
                                "construct a weighted call graph of a module");

void WeightedCallGraphPass::AnalyseCallSite(CallSite &cs) {
	Function *caller, *callee;
	struct called c;
	if(cs.isIndirectCall())
		return;
	caller = cs.getCaller();
	callee = cs.getCalledFunction();
	if (callee->isIntrinsic())
		return;
	//c = new struct called;
	if (this->function_list.count(callee->getName()) == 0)
		Analyse(*callee);
	c.f = this->function_list[callee->getName()];
	c.f->weight++;
	const DebugLoc debugloc = cs.getInstruction()->getDebugLoc();
	c.line = debugloc.getLine();
	c.file = cs.getInstruction()->getModule()->getSourceFileName().c_str();
	this->function_list[caller->getName()]->call_list.push_back(c);
}


// At this point we assume that all we know is that we
// never encounter this function before.
// no more assumptions
void WeightedCallGraphPass::Analyse(Function &f) {
	struct function *fun;
	if (f.isIntrinsic())
		return;
	fun = new function;
	fun->name = f.getName().data();
	fun->weight=0;
	this->function_list.insert({f.getName().data(), fun});
	for (BasicBlock &bb : f) {
		for (Instruction &i : bb) {
			CallSite cs(&i);
			if (!cs.getInstruction())
				continue;
			AnalyseCallSite(cs);

		}
	}
}


// For an analysis pass, runOnModule should perform the actual analysis and
// compute the call graph. The actual output, however, is produced separately
// by the print function.
bool
WeightedCallGraphPass::runOnModule(Module &m) {

  for (Function &fun : m){
    if (this->function_list.count(fun.getName()) == 0)
	    Analyse(fun);
  }
  return false;
}


void
WeightedCallGraphPass::print(raw_ostream &out, const Module *m) const {
  out << "digraph {\n  node [shape=record];\n";

  // Print out all function nodes
  for (auto const &kv: this->function_list) {
    struct function *func = kv.second;
    out << "  \"" << func->name
        << "\"[label=\"{" << func->name
        << "|Weight : " << func->weight;

    unsigned lineID = 0;
    for (auto const &callsite: func->call_list) {
      out << "|<l" << lineID << ">"
          << callsite.file << " : "
          << callsite.line;
      ++lineID;
    }
    out << "}\"];\n";
  }

  // Print the edges between them
  for (auto const &kv: this->function_list) {
    struct function *func = kv.second;
    unsigned lineID = 0;
    //for (auto const& function: this->function_list) {
    for (auto const &callsite: func->call_list) {
      out << "  " << func->name
          << ":l" << lineID
          << " -> " << callsite.f->name<< ";\n";
      //}
      ++lineID;
    }
  }

  out << "}\n";
}

