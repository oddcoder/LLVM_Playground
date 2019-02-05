
#include "llvm/AsmParser/Parser.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/Bitcode/BitcodeReader.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/ErrorHandling.h"

#include <memory>
#include <string>

#include "CallGraph.h"


using namespace std;
using namespace llvm;


static LLVMContext MainContext;

// TODO: This is a temporary placeholder until make_unique ships widely.
template<typename T, typename... Args>
unique_ptr<T> make_unique(Args&&... args) {
    return unique_ptr<T>(new T(forward<Args>(args)...));
}


namespace {

cl::opt<string>
inPath(cl::Positional,
       cl::desc("<Module to analyze>"),
       cl::value_desc("bitcode filename"), cl::Required);
}


int
main (int argc, char **argv, const char **env) {
  // This boilerplate provides convenient stack traces and clean LLVM exit
  // handling. It also initializes the built in support for convenient
  // command line option handling.
  sys::PrintStackTraceOnErrorSignal(argv[0]);
  llvm::PrettyStackTraceProgram X(argc, argv);
  llvm_shutdown_obj shutdown;
  cl::ParseCommandLineOptions(argc, argv);

  // Construct an IR file from the filename passed on the command line.
  LLVMContext &context = MainContext;
  SMDiagnostic err;
  unique_ptr<Module> module;
  module = parseIRFile(inPath.getValue(), err, context);

  if (!module.get()) {
    errs() << "Error reading bitcode file.\n";
    err.print(argv[0], errs());
    return -1;
  }

  // Build up all of the passes that we want to run on the module.
  PassBuilder PB;

  ModulePassManager MPM(true);
  
  LoopAnalysisManager LAM(false);
  FunctionAnalysisManager FAM(true);
  CGSCCAnalysisManager CAM(false);
  ModuleAnalysisManager MAM(true);

  AAManager AA ;//= PB.buildDefaultAAPipeline();
  if (auto Err = PB.parseAAPipeline(AA, "basicaa"))
    report_fatal_error("Error parsing basicaa AA pipeline");
  FAM.registerPass([&] {return std::move(AA);});
  MAM.registerPass([&] {return callgraphs::WeightedCallGraph();});

  PB.registerModuleAnalyses(MAM);
  PB.registerCGSCCAnalyses(CAM);
  PB.registerFunctionAnalyses(FAM);
  PB.registerLoopAnalyses(LAM);
  PB.crossRegisterProxies(LAM, FAM, CAM, MAM);

  MPM.addPass(callgraphs::WeightedCallGraphPrinter(outs()));
  MPM.run(*module, MAM);
  //callgraphs::WeightedCallGraphPrinter Printer(outs());
  //Printer.run(*module, MAM);
  return 0;
}

