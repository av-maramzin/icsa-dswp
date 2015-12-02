#ifndef ICSA_DSWP_CDG_H
#define ICSA_DSWP_CDG_H

#include "llvm/Support/raw_ostream.h"
using llvm::raw_ostream;
#include "llvm/IR/BasicBlock.h"
using llvm::BasicBlock;

#include "Dependence.h"

namespace icsa {

typedef Dependence<BasicBlock> ControlDependenceNode;

class ControlDependenceGraph : public DependenceGraph<BasicBlock> {
public:
  static char ID;
  ControlDependenceGraph() : DependenceGraph<BasicBlock>(ID) {}

  bool runOnFunction(Function &F) override;

  void getAnalysisUsage(AnalysisUsage &Info) const override {
    Info.addRequired<PostDominatorTree>();
  }

  const Function *getFunction() const { return firstValue->getParent(); }

  void releaseMemory() override;

  void print(raw_ostream &OS, const Module *) const override;
};

typedef DependenceGraphBaseIterator<BasicBlock, ControlDependenceNode>
    cdg_iterator;
typedef DependenceGraphBaseIterator<BasicBlock, const ControlDependenceNode>
    cdg_const_iterator;
}

#endif
