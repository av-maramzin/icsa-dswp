#include <string>
using std::string;

#include "llvm/IR/Function.h"
using llvm::Function;
#include "llvm/Pass.h"
using llvm::FunctionPass;
#include "llvm/PassSupport.h"
using llvm::RegisterPass;
#include "llvm/PassAnalysisSupport.h"
using llvm::AnalysisUsage;

#include "FuncGraphTraits.h"
#include "GraphPrinterPass.h"

namespace icsa {

struct CFGPrinter : public GraphPrinterPass<PrintFunction> {
  static char ID;
  PrintFunction PF;
  CFGPrinter() : GraphPrinterPass(ID) {}

  const PrintFunction *getGraphPointer(Function &F) override {
    PF.F = &F;
    return &PF;
  }

  string getPrefix() const override { return "cfg"; }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
  }
};

char CFGPrinter::ID = 0;
RegisterPass<CFGPrinter>
    CFGPrinterRegister("icsa-dot-cfg", "Print CDG of function to 'dot' file");
}
