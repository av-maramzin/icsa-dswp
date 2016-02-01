#include "MDG.h"

#include <iostream>
using std::cout;

#include <memory>
using std::unique_ptr;
#include <map>
using std::map;
#include <vector>
using std::vector;
#include <set>
using std::set;
#include <utility>
using std::move;

#include "llvm/PassSupport.h"
using llvm::RegisterPass;
#include "llvm/Analysis/MemoryDependenceAnalysis.h"
using llvm::MemoryDependenceAnalysis;
using llvm::MemDepResult;
#include "llvm/IR/Function.h"
using llvm::Function;

#include "Util.h"

namespace icsa {

char MemoryDependenceGraphPass::ID = 0;
RegisterPass<MemoryDependenceGraphPass>
    MDGRegister("mdg", "Build Memory Dependence Graph");

bool MemoryDependenceGraphPass::runOnFunction(Function &F) {
  MDG.firstValue = F.getEntryBlock().begin();

  MemoryDependenceAnalysis &mda = Pass::getAnalysis<MemoryDependenceAnalysis>();

  // Add nodes.
  for (auto it = F.begin(); it != F.end(); ++it) {
    const BasicBlock& BB = *it;
    for (auto jt = BB.begin(); jt != BB.end(); ++jt) {
      const Instruction& I = *jt;
      MDG.addNode(const_cast<Instruction*>(&I));
    }
  }

  // Add edges.
  for (auto it = F.begin(); it != F.end(); ++it) {
    const BasicBlock& BB = *it;
    for (auto jt = BB.begin(); jt != BB.end(); ++jt) {
      Instruction* I = const_cast<Instruction*>(&*jt);
      if (!I->mayReadOrWriteMemory()) {
        continue;
      }

      MemDepResult mdr = mda.getDependency(I);
      Instruction *DepInst = mdr.getInst();
      if (DepInst != nullptr && mdr.isDef()) {
        MDG.addEdge(MDG.find(DepInst), MDG.find(I));
      }
    }
  }

  return true;
}

}
