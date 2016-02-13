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

#include "MDG.h"
using icsa::MemoryDependenceGraphPass;

#include "DependenceTraits.h"
using icsa::DepGraphTraitsWrapper;

#include "Util.h"
using icsa::instructionToFunctionName;

#include "InstDOTTraits.h"
// Defines DOTGraphTraits for DepGraphTraitsWrapper<Instruction>.

namespace icsa {

struct MDGPrinter : public FunctionPass {
  static char ID;
  MDGPrinter() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    MemoryDependenceGraphPass &mdg =
        Pass::getAnalysis<MemoryDependenceGraphPass>();
    DependenceGraph<Instruction> G = mdg.getMDG();
    string FuncName = instructionToFunctionName(*G.nodes_begin()->first);
    DepGraphTraitsWrapper<Instruction>(G)
        .writeToFile("mdg." + FuncName + ".dot");
    return false;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
    AU.addRequired<MemoryDependenceGraphPass>();
  }
};

char MDGPrinter::ID = 0;
RegisterPass<MDGPrinter>
    MDGPrinterRegister("dot-mdg", "Print MDG of function to 'dot' file");
}
