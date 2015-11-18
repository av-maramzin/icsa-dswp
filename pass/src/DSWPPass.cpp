#include "DSWPPass.h"

#include "llvm/PassSupport.h"
using llvm::RegisterPass;

#include <iostream>
using std::cout;

// TODO(Stanm): is 0 correct?
char DSWPPass::ID = 0;
RegisterPass<DSWPPass> X(
  "icsa-dswp",
  "ICSA implementation of Decoupled Software Pipeline"
);

DSWPPass::DSWPPass() : LoopPass(ID) {}

void DSWPPass::getAnalysisUsage(AnalysisUsage &Info) const {
  return;
}

bool DSWPPass::runOnLoop(Loop *L, LPPassManager &LPM) {
  cout << "[DSWP] Running...\n";
  cout << "[DSWP] Finished\n";
  return true;
}

bool DSWPPass::doInitialization(Loop *L, LPPassManager &LPM) {
  return true;
}
