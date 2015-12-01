#include "DSWPPass.h"

#include "llvm/PassSupport.h"
using llvm::RegisterPass;
#include "llvm/Analysis/MemoryDependenceAnalysis.h"
using llvm::MemoryDependenceAnalysis;
using llvm::MemDepResult;
#include "llvm/IR/BasicBlock.h"
using llvm::BasicBlock;
#include "llvm/IR/Instruction.h"
using llvm::Instruction;

#include <iostream>
using std::cout;
#include <string>
using std::string;

#include "Util.h"

namespace icsa {

char DSWPPass::ID = 0;
RegisterPass<DSWPPass> X("icsa-dswp",
                         "ICSA implementation of Decoupled Software Pipeline");

DSWPPass::DSWPPass() : LoopPass(ID) {}

void DSWPPass::getAnalysisUsage(AnalysisUsage &Info) const {
  Info.addRequired<MemoryDependenceAnalysis>();
}

bool DSWPPass::runOnLoop(Loop *L, LPPassManager &LPM) {
  // Don't run pass on nested loops.
  if (L->getLoopDepth() > 1) {
    return false;
  }

  // TODO(Stan): implement :)

  return true;
}

bool DSWPPass::doInitialization(Loop *L, LPPassManager &LPM) { return true; }

}
