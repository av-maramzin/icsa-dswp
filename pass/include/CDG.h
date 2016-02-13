#ifndef ICSA_DSWP_CDG_H
#define ICSA_DSWP_CDG_H

#include "llvm/IR/Function.h"
using llvm::Function;
#include "llvm/IR/BasicBlock.h"
using llvm::BasicBlock;

#include "llvm/Pass.h"
using llvm::FunctionPass;
#include "llvm/PassAnalysisSupport.h"
using llvm::AnalysisUsage;

#include "llvm/Analysis/PostDominators.h"
using llvm::PostDominatorTree;

#include "Dependence.h"
using icsa::DependenceGraph;

namespace icsa {

// Designed after ControlDependenceGraphPass.
class ControlDependenceGraphPass : public FunctionPass {
private:
  DependenceGraph<BasicBlock> CDG;

public:
  static char ID;

  ControlDependenceGraphPass() : FunctionPass(ID) {}

  // Entry point of `FunctionPass`.
  bool runOnFunction(Function &F) override;

  // Specifies passes this one depends on.
  void getAnalysisUsage(AnalysisUsage &Info) const override {
    Info.setPreservesAll();
    Info.addRequired<PostDominatorTree>();
  }

  const char *getPassName() const override { return "Control Dependence Graph"; }

  void releaseMemory() override { CDG.clear(); }

  // Get the dependence graph.
  const DependenceGraph<BasicBlock> &getCDG() const { return CDG; }
};
}

#endif
