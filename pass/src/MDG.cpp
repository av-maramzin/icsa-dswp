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
#include "llvm/IR/InstIterator.h"
using llvm::inst_iterator;

#include "Util.h"

namespace icsa {

char MemoryDependenceGraphPass::ID = 0;
RegisterPass<MemoryDependenceGraphPass>
    MDGRegister("mdg", "Build Memory Dependence Graph");

bool MemoryDependenceGraphPass::runOnFunction(Function &F) {
  MemoryDependenceAnalysis &mda = Pass::getAnalysis<MemoryDependenceAnalysis>();

  // Add nodes.
  for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
    Instruction *Inst = &*I;
    MDG.addNode(Inst);
  }

  // Add edges.
  for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
    Instruction *Inst = &*I;
    if (!Inst->mayReadOrWriteMemory()) {
      continue;
    }

    MemDepResult mdr = mda.getDependency(Inst);
    Instruction *DepInst = mdr.getInst();
    if (DepInst != nullptr && mdr.isDef()) {
      // Add dependence only if it's a def dependence.
      MDG.addEdge(DepInst, Inst);
    }
  }

  return false;
}
}
