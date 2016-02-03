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

#include "CDG.h"
using icsa::ControlDependenceGraphPass;

#include "DependenceTraits.h"
using icsa::DepGraphTraitsWrapper;

#include "Util.h"
using icsa::instructionToFunctionName;

#include "BasicBlockDOTTraits.h"
// Defines DOTGraphTraits for DepGraphTraitsWrapper<BasicBlock>.

namespace icsa {

struct CDGPrinter : public FunctionPass {
  static char ID;
  CDGPrinter() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    ControlDependenceGraphPass &cdg =
        Pass::getAnalysis<ControlDependenceGraphPass>();
    DependenceGraph<BasicBlock> G = cdg.getCDG();
    string FuncName = basicBlockToFunctionName(*G.nodes_begin()->first);
    DepGraphTraitsWrapper<BasicBlock>(G)
        .writeToFile("cdg." + FuncName + ".dot");
    return false;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
    AU.addRequired<ControlDependenceGraphPass>();
  }
};

char CDGPrinter::ID = 0;
RegisterPass<CDGPrinter>
    CDGPrinterRegister("dot-cdg", "Print CDG of function to 'dot' file");
}
