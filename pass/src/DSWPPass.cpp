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

char DSWPPass::ID = 0;
RegisterPass<DSWPPass> X(
  "icsa-dswp",
  "ICSA implementation of Decoupled Software Pipeline"
);

DSWPPass::DSWPPass() : LoopPass(ID) {}

void DSWPPass::getAnalysisUsage(AnalysisUsage &Info) const {
  Info.addRequired<MemoryDependenceAnalysis>();
}

bool DSWPPass::runOnLoop(Loop *L, LPPassManager &LPM) {
  LOG_DEBUG("loop depth ", L->getLoopDepth());

  // Don't run pass on nested loops.
  if (L->getLoopDepth() > 1) {
    LOG_INFO("Skipping nested loop.");
    return false;
  }
  LOG_INFO("Running...");

  MemoryDependenceAnalysis &mda = Pass::getAnalysis<MemoryDependenceAnalysis>();
  for (Loop::block_iterator bi = L->getBlocks().begin();
       bi != L->getBlocks().end(); bi++) {
    BasicBlock *BB = *bi;
    LOG_DEBUG("basic block ", BB);
    for (BasicBlock::iterator ii = BB->begin(); ii != BB->end(); ii++) {
      Instruction *inst = &(*ii);

      if (!inst->mayReadOrWriteMemory()) {
        // Only memory instructions are valid input for the MDA pass.
        continue;
      }

      Instruction *dep = mda.getDependency(inst).getInst();
      if (nullptr != dep) {
        LOG_DEBUG(dep);
        LOG_DEBUG("  |");
        LOG_DEBUG("  v");
        LOG_DEBUG(inst);
      }
    }
  }

  LOG_INFO("Finished.");
  return true;
}

bool DSWPPass::doInitialization(Loop *L, LPPassManager &LPM) {
  return true;
}
