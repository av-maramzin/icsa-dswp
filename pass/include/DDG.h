#ifndef ICSA_DSWP_DDG_H
#define ICSA_DSWP_DDG_H

#include "llvm/Support/raw_ostream.h"
using llvm::raw_ostream;

#include "llvm/IR/Module.h"
using llvm::Module;
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

namespace icsa {

// Designed after ControlDependenceGraphPass.
class DataDependenceGraphPass : public FunctionPass {
private:
  DependenceGraph<Instruction> DDG;

public:
  static char ID;

  DataDependenceGraphPass() : FunctionPass(ID) {}

  // Entry point of `FunctionPass`.
  bool runOnFunction(Function &F) override;

  // Get the dependence graph.
  const DependenceGraph<Instruction> &getDDG() const { return DDG; }

  // Specifies passes this one depends on.
  void getAnalysisUsage(AnalysisUsage &Info) const override {
    Info.setPreservesAll();
  }

  const char *getPassName() const override {
    return "Data Dependence Graph";
  }

  void releaseMemory() override { DDG.clear(); }
};

}

#endif
