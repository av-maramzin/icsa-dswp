// Implemented in DSWPPass.cpp.
#ifndef ICSA_DSWP_PASS_H
#define ICSA_DSWP_PASS_H

#include "llvm/Analysis/LoopInfo.h"
using llvm::Loop;
#include "llvm/Analysis/LoopPass.h"
using llvm::LoopPass;
using llvm::LPPassManager;
#include "llvm/PassAnalysisSupport.h"
using llvm::AnalysisUsage;

#include <string>
using std::string;

namespace icsa {

class DSWPPass : public LoopPass {
public:
  static char ID;
  DSWPPass();
  void getAnalysisUsage(AnalysisUsage &Info) const override;
  bool runOnLoop(Loop *L, LPPassManager &LPM) override;
  bool doInitialization(Loop *L, LPPassManager &LPM) override;
};

}

#endif
