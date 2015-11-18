// Implemented in DSWPPass.cpp.
#ifndef DSWPPASS_H
#define DSWPPASS_H

#include "llvm/Analysis/LoopInfo.h"
using llvm::Loop;
#include "llvm/Analysis/LoopPass.h"
using llvm::LoopPass;
using llvm::LPPassManager;
#include "llvm/PassAnalysisSupport.h"
using llvm::AnalysisUsage;

class DSWPPass : public LoopPass {
public:
  static char ID;
  DSWPPass();
  void getAnalysisUsage(AnalysisUsage &Info) const override;
  bool runOnLoop(Loop *L, LPPassManager &LPM) override;
  bool doInitialization(Loop *L, LPPassManager &LPM) override;
};

#endif
