#ifndef ICSA_DSWP_DECOUPLE_LOOPS_H
#define ICSA_DSWP_DECOUPLE_LOOPS_H

#include "llvm/IR/Function.h"
using llvm::Function;

#include "llvm/Analysis/LoopInfo.h"
using llvm::Loop;
using llvm::LoopInfo;

#include "llvm/Pass.h"
using llvm::FunctionPass;
#include "llvm/PassAnalysisSupport.h"
using llvm::AnalysisUsage;

#include "PSG.h"
using icsa::PDGSCCGraphPass;

namespace icsa {

class DecoupleLoopsPass : public FunctionPass {
private:
  LoopInfo LI;
  map<const Loop *, set<const set<const Instruction *> *>> LoopToIterScc;
  map<const Loop *, set<const set<const Instruction *> *>> LoopToWorkScc;

public:
  static char ID;

  DecoupleLoopsPass() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override;

  void getAnalysisUsage(AnalysisUsage &Info) const override {
    Info.setPreservesAll();
    Info.addRequired<PDGSCCGraphPass>();
  }

  const LoopInfo &getLI() { return LI; }
  const map<const Loop *, set<const set<const Instruction *> *>> &
  getLoopToIterScc() {
    return LoopToIterScc;
  }
  const map<const Loop *, set<const set<const Instruction *> *>> &
  getLoopToWorkScc() {
    return LoopToWorkScc;
  }
};
}

#endif
