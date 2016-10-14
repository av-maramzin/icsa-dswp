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
  PDGSCCGraphPass *PSGP;

public:
  static char ID;

  DecoupleLoopsPass() : FunctionPass(ID), PSGP(nullptr) {}

  bool runOnFunction(Function &F) override;

  void getAnalysisUsage(AnalysisUsage &Info) const override {
    Info.setPreservesAll();
    Info.addRequired<LoopInfoWrapperPass>();
    Info.addRequired<PDGSCCGraphPass>();
  }

  const LoopInfo *getLI(const Function *F) const {
    const auto &found = LI.find(F);
    if (LI.end() == found) return nullptr;
    return found->second;
  }

  LoopInfo *getLI(const Function *F) {
    return const_cast<LoopInfo *>(static_cast<const DecoupleLoopsPass *>(this)->getLI(F));
  }

  const map<const Loop *, set<const set<const Instruction *> *>> &
  getLoopToIterScc() {
    return LoopToIterScc;
  }
  const map<const Loop *, set<const set<const Instruction *> *>> &
  getLoopToWorkScc() {
    return LoopToWorkScc;
  }

  // Is Inst a work instruction in the loop L?
  bool isWork(const Instruction &Inst, const Loop *L) const;
  bool isWork(const Instruction &Inst, const Loop *L);

  // Is Inst an iter instruction in the loop L?
  bool isIter(const Instruction &Inst, const Loop *L) const;
  bool isIter(const Instruction &Inst, const Loop *L);

  // Does loop L have work instructions?
  bool hasWork(const Loop *L) const {
    const auto &found = LoopToWorkScc.find(L);
    if (LoopToWorkScc.end() == found) return false;
    return (found->second).size() != 0;
  }

  bool hasWork(const Loop *L) {
    return static_cast<const DecoupleLoopsPass *>(this)->hasWork(L);
  }

  void releaseMemory() override {
    LoopToWorkScc.clear();
    LoopToIterScc.clear();

    return;
  }

};
}

#endif
