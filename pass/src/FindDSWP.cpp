#include "FindDSWP.h"

#include <iostream>
using std::cout;

#include <map>
using std::map;
using std::pair;

#include <vector>
using std::vector;

#include <set>
using std::set;

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

namespace icsa {

char FindDSWP::ID = 0;
RegisterPass<FindDSWP>
    FindDSWPRegister("find-dswp",
                     "Find Decoupled Software Pipeline opportunities");

bool FindDSWP::runOnFunction(Function &F) {
  typedef GraphUtils<ProgramDependenceGraph> GUtils;
  ProgramDependenceGraphPass &pdgp =
      Pass::getAnalysis<ProgramDependenceGraphPass>();
  const ProgramDependenceGraph &PDG = pdgp.getPDG();

  auto sccs = GUtils::findSCC(PDG);

  const ProgramDependenceNode *root = GUtils::getNonTrivialSCC(sccs);
  if (root == nullptr) {
    return false;
  }

  // Find the loop containing the SCC
  DominatorTree DT;
  DT.recalculate(F);

  LoopInfo LI;
  LI.Analyze(DT);

  Loop *loop = nullptr;
  for (LoopInfo::iterator I = LI.begin(), E = LI.end();
       loop == nullptr && I != E; ++I) {
    Loop *L = *I;
    for (Loop::block_iterator BI = L->block_begin(), BE = L->block_end();
         loop == nullptr && BI != BE; ++BI) {
      BasicBlock *BB = *BI;
      for (BasicBlock::iterator II = BB->begin(), IE = BB->end();
           loop == nullptr && II != IE; ++II) {
        if (static_cast<Instruction *>(II) == root->getValue()) {
          loop = L;
        }
      }
    }
  }

  raw_os_ostream roos(cout);
  if (loop == nullptr) {
    roos << "ERROR: could not find enclosing loop!\n";
    return false;
  }
  // iterator instructions: sccs[root]
  // payload instructions: loop - sccs[root]
  vector<const Instruction *> iteration;
  vector<const Instruction *> payload;

  for (auto dep : sccs[root]) {
    iteration.push_back(dep->getValue());
  }

  for (LoopInfo::iterator I = LI.begin(), E = LI.end(); I != E; ++I) {
    Loop *L = *I;
    for (Loop::block_iterator BI = L->block_begin(), BE = L->block_end();
         BI != BE; ++BI) {
      BasicBlock *BB = *BI;
      for (BasicBlock::iterator II = BB->begin(), IE = BB->end(); II != IE;
           ++II) {
        Instruction *Inst = static_cast<Instruction *>(II);
        if (find(iteration.begin(), iteration.end(), Inst) == iteration.end()) {
          payload.push_back(Inst);
        }
      }
    }
  }

  bool meaningfull_payload = false;
  for (auto payload_instruction : payload) {
    const BranchInst *BI = dyn_cast<BranchInst>(payload_instruction);
    if (nullptr == BI || !BI->isUnconditional()) {
      meaningfull_payload = true;
      break;
    }
  }
  if (!meaningfull_payload) {
    // completely entangled loop
    return false;
  }

  roos << "Found a DSWP opportunity!\n\n";

  roos << "Loop:\n";
  for (LoopInfo::iterator I = LI.begin(), E = LI.end(); I != E; ++I) {
    Loop *L = *I;
    for (Loop::block_iterator BI = L->block_begin(), BE = L->block_end();
         BI != BE; ++BI) {
      BasicBlock *BB = *BI;
      BB->print(roos);
    }
  }
  roos << '\n';

  roos << "Iterator instructions: \n";
  for (auto iterator_instruction : iteration) {
    iterator_instruction->print(roos);
    roos << '\n';
  }
  roos << '\n';
  roos << "Payload instructions: \n";
  for (auto payload_instruction : payload) {
    payload_instruction->print(roos);
    roos << '\n';
  }
  roos << '\n';

  return false;
}
}
