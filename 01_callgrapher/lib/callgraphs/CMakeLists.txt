add_library(callgraphs CallGraph.cpp)

add_llvm_loadable_module(callgraphs_plugin CallGraph.cpp)
