
#include "CallGraph.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/Support/raw_ostream.h"

#include <iterator>

using namespace llvm;
using namespace callgraphs;


char WeightedCallGraphPass::ID = 0;

RegisterPass<WeightedCallGraphPass> X("weightedcg",
                                "construct a weighted call graph of a module");


// For an analysis pass, runOnModule should perform the actual analysis and
// compute the call graph. The actual output, however, is produced separately
// by the print function.
bool
WeightedCallGraphPass::runOnModule(Module &m) {

  // NOTE: This is the entry point for whatever changes you'd like to make in
  // order to build a call graph.
  
  return false;
}


void
WeightedCallGraphPass::print(raw_ostream &out, const Module *m) const {
  out << "digraph {\n  node [shape=record];\n";

  // Print out all function nodes
  for (auto const& function: this->function_list) {
    out << "  " << function.name
        << "[label=\"{" << function.name
        << "|Weight: " << function.weight;

    unsigned lineID = 0;
    for (auto const& callsite: function.call_list) {
      out << "|<l" << lineID << ">"
          << callsite.file << ":"
          << callsite.f->name;
      ++lineID;
    }
    out << "}\"];\n";
  }

  // Print the edges between them
  for (auto const& function: this->function_list) {
    unsigned lineID = 0;
    //for (auto const& function: this->function_list) {
    for (auto const& callsite: function.call_list) {
      out << "  " << function.name
          << ":l" << lineID
          << " -> " << callsite.f->name<< ";\n";
      //}
      +lineID;
    }
  }

  out << "}\n";
}

