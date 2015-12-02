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

#include "CDG.h"
#include "CDGGraphTraits.h"
#include "GraphPrinterPass.h"

namespace icsa {

struct CDGPrinter : public GraphPrinterPass<ControlDependenceGraph> {
  static char ID;
  const ControlDependenceGraph &CDG;
  CDGPrinter()
      : CDG(Pass::getAnalysis<ControlDependenceGraph>()), GraphPrinterPass(ID) {
  }

  const ControlDependenceGraph *getGraphPointer(Function &) override {
    return &CDG;
  }

  string getPrefix() const override { return "cdg"; }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
    AU.addRequired<ControlDependenceGraph>();
  }
};

char CDGPrinter::ID = 0;
RegisterPass<CDGPrinter>
    CDGPrinterRegister("dot-cdg", "Print CDG of function to 'dot' file");
}
