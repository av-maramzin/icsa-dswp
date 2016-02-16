#include "DecoupleLoops.h"

#include <iostream>
using std::cout;

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

#include "llvm/Support/Casting.h"
using llvm::dyn_cast;

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

  const PDGSCCGraphPass &PSGP = Pass::getAnalysis<PDGSCCGraphPass>();
  const DependenceGraph<set<const Instruction *>> &PSG = PSGP.getPSG();

  DominatorTree DT;
  DT.recalculate(F);

  LoopInfo LI;
  LI.Analyze(DT);

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
  for (Loop *L : LI) {
    for (Loop::block_iterator BI = L->block_begin(), BE = L->block_end();
         BI != BE; ++BI) {
      BasicBlock *BB = *BI;
      for (Instruction &Inst : *BB) {
        SccToLoop[PSGP.getSCC(&Inst)] = L;
      }
    }
  }

  set<SccConstPtr> workScc;
  set<SccConstPtr> iterScc;
  // 2: for all SCC S1:
  //      if in loop: call the loop L1
  //        for all SCC S2, s.t. S1 depends on S2:
  //          if S2 is in L1: S1 is work; break loop
  //        if S1 is not work: S1 is iterator
  auto TPSG = GraphUtils<set<const Instruction *>>::transpose(PSG);
  for (auto I = PSG.nodes_cbegin(), E = PSG.nodes_cend(); I != E; ++I) {
    SccConstPtr S1 = I->first;
    auto S1LoopPair = SccToLoop.find(S1);
    if (S1LoopPair == SccToLoop.end()) {
      continue;
    }
    for (auto J = TPSG.child_cbegin(S1), F = TPSG.child_cend(S1); J != F; ++J) {
      SccConstPtr S2 = *J;
      auto S2LoopPair = SccToLoop.find(S2);
      if (S2LoopPair != SccToLoop.end() &&
          S1LoopPair->second == S2LoopPair->second) {
        workScc.insert(S1);
        break;
      }
    }
    if (workScc.find(S1) == workScc.end()) {
      iterScc.insert(S1);
    }
  }

  // 3: LoopInst -> work / iter
  set<const Instruction *> workInst;
  set<const Instruction *> iterInst;
  for (Loop *L : LI) {
    for (Loop::block_iterator BI = L->block_begin(), BE = L->block_end();
         BI != BE; ++BI) {
      BasicBlock *BB = *BI;
      for (Instruction &Inst : *BB) {
        SccConstPtr SCC = PSGP.getSCC(&Inst);
        if (workScc.find(SCC) != workScc.end()) {
          workInst.insert(&Inst);
        }
        if (iterScc.find(SCC) != iterScc.end()) {
          iterInst.insert(&Inst);
        }
      }
    }
  }

  // tmp: print
  raw_os_ostream roos(cout);
  roos << "Function " << F.getName() << ":\n\n";
  int count = 1;
  for (Loop *L : LI) {
    roos << "Loop " << count++ << ":\n";
    for (Loop::block_iterator BI = L->block_begin(), BE = L->block_end();
         BI != BE; ++BI) {
      BasicBlock *BB = *BI;
      roos << '\n';
      BB->printAsOperand(roos);
      roos << '\n';
      for (Instruction &Inst : *BB) {
        Inst.print(roos);
        if (workInst.find(&Inst) != workInst.end()) {
          roos << " ; work";
        }
        if (iterInst.find(&Inst) != iterInst.end()) {
          roos << " ; iter";
        }
        roos << '\n';
      }
    }
  }

  return false;
}
}
