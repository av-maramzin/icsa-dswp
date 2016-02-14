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

#include "PDG.h"
using icsa::ProgramDependenceGraphPass;

#include "DependenceTraits.h"
using icsa::DepGraphTraitsWrapper;

#include "Util.h"
using icsa::instructionToFunctionName;

#include "InstDOTTraits.h"
// Defines DOTGraphTraits for DepGraphTraitsWrapper<Instruction>.

namespace icsa {

struct PDGPrinter : public FunctionPass {
  static char ID;
  PDGPrinter() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    ProgramDependenceGraphPass &pdg =
        Pass::getAnalysis<ProgramDependenceGraphPass>();
    DependenceGraph<Instruction> G = pdg.getPDG();
    string FuncName = instructionToFunctionName(*G.nodes_begin()->first);
    DepGraphTraitsWrapper<Instruction>(G)
        .writeToFile("pdg." + FuncName + ".dot");
    return false;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
    AU.addRequired<ProgramDependenceGraphPass>();
  }
};

char PDGPrinter::ID = 0;
RegisterPass<PDGPrinter>
    PDGPrinterRegister("dot-pdg", "Print PDG of function to 'dot' file");
}
