
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


// Output for a pure analysis pass should happen in the print method.
// It is called automatically after the analysis pass has finished collecting
// its information.
void
WeightedCallGraphPass::print(raw_ostream &out, const Module *m) const {
  out << "digraph {\n  node [shape=record];\n";

  // Print out all function nodes
  for (/* Iterate through all functions */) {
    out << "  " << "TODO" /* Print out the function name here. */
        << "[label=\"{" << "TODO" /* Print out the function name here. */
        << "|Weight: " << "TODO" /* The weight of this function */;

    unsigned lineID = 0;
    for (/* For each call site in the function */) {
      out << "|<l" << lineID << ">"
          << "TODO" /* The file name of the callsite */ << ":"
          << "TODO" /* The line number */;
      ++lineID;
    }
    out << "}\"];\n";
  }

  // Print the edges between them
  for (/* Iterate through all functions */) {
    unsigned lineID = 0;
    for (/* For each call site in the function */) {
      for (/* For each possible call target */) {
        out << "  " << "TODO" /* ID for the caller function */
            << ":l" << lineID
            << " -> " << "TODO" /* ID for the callee function */ << ";\n";
      }
      ++lineID;
    }
  }

  out << "}\n";
}

