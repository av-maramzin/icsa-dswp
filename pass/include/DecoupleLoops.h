#ifndef ICSA_DSWP_DECOUPLE_LOOPS_H
#define ICSA_DSWP_DECOUPLE_LOOPS_H

#include "llvm/IR/Function.h"
using llvm::Function;

#include "llvm/Analysis/LoopInfo.h"
using llvm::Loop;
using llvm::LoopInfo;
using llvm::LoopInfoWrapperPass;

#include "llvm/Pass.h"
using llvm::FunctionPass;
#include "llvm/PassAnalysisSupport.h"
using llvm::AnalysisUsage;

#include "PSG.h"
using icsa::PDGSCCGraphPass;

namespace icsa {

class DecoupleLoopsPass : public FunctionPass {
private:
  map<const Function *, LoopInfo *> LI;
  map<const Loop *, set<const set<const Instruction *> *>> LoopToIterScc;
  map<const Loop *, set<const set<const Instruction *> *>> LoopToWorkScc;

public:
  static char ID;

  DecoupleLoopsPass() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override;

  void getAnalysisUsage(AnalysisUsage &Info) const override {
    Info.setPreservesAll();
    Info.addRequired<LoopInfoWrapperPass>();
    Info.addRequired<PDGSCCGraphPass>();
  }

  LoopInfo *getLI(const Function *F) { return LI[F]; }
  const map<const Loop *, set<const set<const Instruction *> *>> &
  getLoopToIterScc() {
    return LoopToIterScc;
  }
  const map<const Loop *, set<const set<const Instruction *> *>> &
  getLoopToWorkScc() {
    return LoopToWorkScc;
  }

  // Is Inst a work instruction in the loop L?
  bool isWork(const Instruction &Inst, const Loop *L);

  // Is Inst an iter instruction in the loop L?
  bool isIter(const Instruction &Inst, const Loop *L);

  // Does loop L have work instructions?
  bool hasWork(const Loop *L) {
    return LoopToWorkScc[L].size() != 0;
  }
};
}

#endif
