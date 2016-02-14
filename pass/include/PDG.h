#ifndef ICSA_DSWP_PDG_H
#define ICSA_DSWP_PDG_H

#include "llvm/IR/Function.h"
using llvm::Function;
#include "llvm/IR/Instruction.h"
using llvm::Instruction;

#include "llvm/Pass.h"
using llvm::FunctionPass;
#include "llvm/PassAnalysisSupport.h"
using llvm::AnalysisUsage;

#include "Dependence.h"
using icsa::DependenceGraph;

#include "CDG.h"
using icsa::ControlDependenceGraphPass;
#include "MDG.h"
using icsa::MemoryDependenceGraphPass;
#include "DDG.h"
using icsa::DataDependenceGraphPass;

namespace icsa {

// Designed after ControlDependenceGraphPass.
class ProgramDependenceGraphPass : public FunctionPass {
private:
  DependenceGraph<Instruction> PDG;

public:
  static char ID;

  ProgramDependenceGraphPass() : FunctionPass(ID) {}

  // Entry point of `FunctionPass`.
  bool runOnFunction(Function &F) override;

  // Specifies passes this one depends on.
  void getAnalysisUsage(AnalysisUsage &Info) const override {
    Info.setPreservesAll();
    Info.addRequired<ControlDependenceGraphPass>();
    Info.addRequired<MemoryDependenceGraphPass>();
    Info.addRequired<DataDependenceGraphPass>();
  }

  const char *getPassName() const override { return "Program Dependence Graph"; }

  void releaseMemory() override { PDG.clear(); }

  // Get the dependence graph.
  const DependenceGraph<Instruction> &getPDG() const { return PDG; }
};
}

#endif
