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
  // Keeps track of the memory that needs to be cleaned. It's a map from the
  // root of a SCC to the SCC itself.
  map<const Instruction *, set<const Instruction *>> SCCs;

  DependenceGraph<set<const Instruction *>> PSG;
  // Maps instructions to their SCC.
  map<const Instruction *, const set<const Instruction *> *> InstToSCC;

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

  const set<const Instruction *> *getSCC(const Instruction* Inst) const {
    return InstToSCC.at(Inst);
  }
};
}

#endif
