#ifndef ICSA_DSWP_MDG_H
#define ICSA_DSWP_MDG_H

#include "llvm/IR/Function.h"
using llvm::Function;
#include "llvm/IR/Instruction.h"
using llvm::Instruction;

#include "llvm/Pass.h"
using llvm::FunctionPass;
#include "llvm/PassAnalysisSupport.h"
using llvm::AnalysisUsage;

#include "llvm/Analysis/MemoryDependenceAnalysis.h"
using llvm::MemoryDependenceAnalysis;

#include "Dependence.h"
using icsa::DependenceGraph;

namespace icsa {

// Designed after ControlDependenceGraphPass.
class MemoryDependenceGraphPass : public FunctionPass {
private:
  DependenceGraph<Instruction> MDG;

public:
  static char ID;

  MemoryDependenceGraphPass() : FunctionPass(ID) {}

  // Entry point of `FunctionPass`.
  bool runOnFunction(Function &F) override;

  // Specifies passes this one depends on.
  void getAnalysisUsage(AnalysisUsage &Info) const override {
    Info.setPreservesAll();
    Info.addRequired<MemoryDependenceAnalysis>();
  }

  const char *getPassName() const override { return "Memory Dependence Graph"; }

  void releaseMemory() override { MDG.clear(); }

  // Get the dependence graph.
  const DependenceGraph<Instruction> &getMDG() const { return MDG; }
};
}

#endif
