#include "DecoupleLoops.h"

#include <map>
using std::map;
using std::pair;

#include <vector>
using std::vector;

#include <set>
using std::set;

#include <string>
using std::string;

#include "llvm/PassSupport.h"
using llvm::RegisterPass;

#include "llvm/Support/raw_os_ostream.h"
using llvm::raw_os_ostream;

#include "PDG.h"
#include "GraphUtils.h"

#include "llvm/Analysis/LoopInfo.h"
using llvm::Loop;
using llvm::LoopInfo;

#include "llvm/IR/Instructions.h"
using llvm::BranchInst;

#include "llvm/IR/Dominators.h"
using llvm::DominatorTree;

#include "llvm/IR/DebugInfoMetadata.h"
using llvm::DILocation;

#include "llvm/ADT/StringRef.h"
using llvm::StringRef;

namespace icsa {

char DecoupleLoopsPass::ID = 0;
RegisterPass<DecoupleLoopsPass> DecoupleLoopsRegister(
    "decouple-loops",
    "Identify the iterator and work instructions of the loops in a function");

bool DecoupleLoopsPass::runOnFunction(Function &F) {
  typedef const set<const Instruction *> *SccConstPtr;

  PSGP = &(Pass::getAnalysis<PDGSCCGraphPass>());
  const DependenceGraph<set<const Instruction *>> &PSG = PSGP->getPSG();

  DominatorTree DT;
  DT.recalculate(F);
  //LI[&F]->Analyze(DT);
  LI[&F] = &(getAnalysis<LoopInfoWrapperPass>().getLoopInfo());
  DT.releaseMemory();

  // Given:
  // Inst -> SCC
  // Loop -> Inst
  //
  // An SCC is iterator, if it is in a loop and all SCCs it depends on are not
  // in the loop.
  // An SCC is work, if it is in a loop and there are SCCs it depends on which
  // are in the same loop.
  //
  // 1: SCC -> Loop

  map<SccConstPtr, const Loop *> SccToLoop;
  for (Loop *L : *(LI[&F])) {
    for (Loop::block_iterator BI = L->block_begin(), BE = L->block_end();
         BI != BE; ++BI) {
      BasicBlock *BB = *BI;
      for (Instruction &Inst : *BB) {
        SccToLoop[PSGP->getSCC(&Inst)] = L;
      }
    }
  }

  // 2: for all SCC S1:
  //      if in loop: call the loop L1
  //        for all SCC S2, s.t. S1 depends on S2:
  //          if S2 is in L1: S1 is work; break loop
  //        if S1 is not work: S1 is iterator
  auto TPSG = GraphUtils<set<const Instruction *>>::transpose(PSG);
  for (auto I = PSG.nodes_cbegin(), E = PSG.nodes_cend(); I != E; ++I) {
    SccConstPtr S1 = I->first;
    auto S1LoopPair = SccToLoop.find(S1);
    auto S1Loop = S1LoopPair->second;
    if (S1LoopPair == SccToLoop.end()) {
      continue;
    }
    for (auto J = TPSG.child_cbegin(S1), F = TPSG.child_cend(S1); J != F; ++J) {
      SccConstPtr S2 = *J;
      auto S2LoopPair = SccToLoop.find(S2);
      if (S2LoopPair != SccToLoop.end() &&
          S1Loop == S2LoopPair->second) {
        LoopToWorkScc[S1Loop].insert(S1);
        break;
      }
    }
    if (LoopToWorkScc[S1Loop].find(S1) == LoopToWorkScc[S1Loop].end()) {
      LoopToIterScc[S1Loop].insert(S1);
    }
  }
  SccToLoop.clear();

  return false;
}

bool DecoupleLoopsPass::isWork(const Instruction &Inst, const Loop *L) const {
  const auto &LWSCC = LoopToWorkScc.find(L);
  if (LWSCC == LoopToWorkScc.end()) return false;
  return (LWSCC->second).find(PSGP->getSCC(&Inst)) != (LWSCC->second).end();
}

bool DecoupleLoopsPass::isWork(const Instruction &Inst, const Loop *L) {
  return static_cast<const DecoupleLoopsPass *>(this)->isWork(Inst, L);
}

bool DecoupleLoopsPass::isIter(const Instruction &Inst, const Loop *L) const {
  const auto&LISCC = LoopToIterScc.find(L);
  if (LISCC == LoopToIterScc.end()) return false;
  return (LISCC->second).find(PSGP->getSCC(&Inst)) != (LISCC->second).end();
}

bool DecoupleLoopsPass::isIter(const Instruction &Inst, const Loop *L) {
  return static_cast<const DecoupleLoopsPass *>(this)->isIter(Inst, L);
}

}

