#ifndef ICSA_DSWP_PSG_H
#define ICSA_DSWP_PSG_H

#include <set>
using std::set;

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

#include "PDG.h"
using icsa::ProgramDependenceGraphPass;

namespace icsa {

// Designed after ControlDependenceGraphPass.
class PDGSCCGraphPass : public FunctionPass {
private:
  DependenceGraph<set<const Instruction *>> PSG;
  // Keeps track of the memory that needs to be cleaned.
  map<const Instruction *, set<const Instruction *>> SCCs;

public:
  static char ID;

  PDGSCCGraphPass() : FunctionPass(ID) {}

  // Entry point of `FunctionPass`.
  bool runOnFunction(Function &F) override;

  // Specifies passes this one depends on.
  void getAnalysisUsage(AnalysisUsage &Info) const override {
    Info.setPreservesAll();
    Info.addRequired<ProgramDependenceGraphPass>();
  }

  const char *getPassName() const override {
    return "PDG Strongly Connected Components Graph";
  }

  void releaseMemory() override {
    PSG.clear();
    SCCs.clear();
  }

  // Get the dependence graph.
  const DependenceGraph<set<const Instruction *>> &getPSG() const {
    return PSG;
  }
};
}

#endif
