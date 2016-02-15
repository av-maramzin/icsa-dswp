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

#include "PSG.h"
using icsa::PDGSCCGraphPass;

#include "DependenceTraits.h"
using icsa::DepGraphTraitsWrapper;

#include "Util.h"
using icsa::instructionToFunctionName;

#include "InstSetDOTTraits.h"
// Defines DOTGraphTraits for DepGraphTraitsWrapper<Instruction>.

namespace icsa {

struct PSGPrinter : public FunctionPass {
  static char ID;
  PSGPrinter() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    PDGSCCGraphPass &psg = Pass::getAnalysis<PDGSCCGraphPass>();
    DependenceGraph<set<const Instruction *>> G = psg.getPSG();
    const set<const Instruction *> *firstSCC = G.nodes_begin()->first;
    string FuncName = instructionToFunctionName(**firstSCC->begin());
    DepGraphTraitsWrapper<set<const Instruction *>>(G)
        .writeToFile("psg." + FuncName + ".dot");
    return false;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
    AU.addRequired<PDGSCCGraphPass>();
  }
};

char PSGPrinter::ID = 0;
RegisterPass<PSGPrinter>
    PSGPrinterRegister("dot-psg", "Print PSG of function to 'dot' file");
}
