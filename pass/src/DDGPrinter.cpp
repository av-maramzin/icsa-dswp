#include <string>
using std::string;
#include <system_error>
using std::error_code;

#include "llvm/IR/Function.h"
using llvm::Function;

#include "llvm/Pass.h"
using llvm::FunctionPass;
#include "llvm/PassRegistry.h"
using llvm::RegisterPass;

#include "llvm/PassAnalysisSupport.h"
using llvm::AnalysisUsage;

#include "DDG.h"
using icsa::DataDependenceGraphPass;

#include "DependenceTraits.h"
using icsa::DepGraphTraitsWrapper;

#include "Util.h"
using icsa::instructionToFunctionName;

#include "InstDOTTraits.h"
// Defines DOTGraphTraits for DepGraphTraitsWrapper<Instruction>.

namespace icsa {

struct DDGPrinter : public FunctionPass {
  static char ID;
  DDGPrinter() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    DataDependenceGraphPass &ddg = Pass::getAnalysis<DataDependenceGraphPass>();
    DependenceGraph<Instruction> G = ddg.getDDG();
    string FuncName = instructionToFunctionName(*G.nodes_begin()->first);
    DepGraphTraitsWrapper<Instruction>(G)
        .writeToFile("ddg." + FuncName + ".dot");
    return false;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
    AU.addRequired<DataDependenceGraphPass>();
  }
};

char DDGPrinter::ID = 0;
RegisterPass<DDGPrinter>
    DDGPrinterRegister("dot-ddg", "Print DDG of function to 'dot' file");
}
