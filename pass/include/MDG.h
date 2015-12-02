#ifndef ICSA_DSWP_MDG_H
#define ICSA_DSWP_MDG_H

#include "llvm/Support/raw_ostream.h"
using llvm::raw_ostream;
#include "llvm/IR/Instruction.h"
using llvm::Instruction;

#include "Dependence.h"

namespace icsa {

typedef Dependence<Instruction> MemoryDependenceNode;

class MemoryDependenceGraph : public DependenceGraph<Instruction> {
public:
  static char ID;
  MemoryDependenceGraph() : DependenceGraph<Instruction>(ID) {
  }

  bool runOnFunction(Function &F) override;

  void getAnalysisUsage(AnalysisUsage &Info) const override {
    Info.addRequired<PostDominatorTree>();
  }

  void releaseMemory() override;

  void print(raw_ostream &OS, const Module *) const override;
};

typedef DependenceGraphBaseIterator<Instruction, MemoryDependenceNode>
    mdg_iterator;
typedef DependenceGraphBaseIterator<Instruction, const MemoryDependenceNode>
    mdg_const_iterator;
}

#endif
