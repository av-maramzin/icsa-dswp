/*******************************************************************************
 *
 * (please update README.md if you modify this comment).
 *
 * The simplest use of the `DependenceGraph` class is in the implementation of
 * the Data Dependence Graph (DDG) LLVM function pass, which builds the DDG of
 * a function using the def-use chains of instructions. This file defines a
 * trivial `FunctionPass` - `DataDependenceGraphPass`. The non-trivial parts of
 * the definition are that it is storing an instance of
 * `DependenceGraph<Instruction>` which can be accessed via a getter method and
 * cleared via `releaseMemory()`, and the declaration inside
 * `getAnalysisUsage()` that the pass doesn't modify the given `LLVM IR` in any
 * way.
 *
*******************************************************************************/
#ifndef ICSA_DSWP_DDG_H
#define ICSA_DSWP_DDG_H

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

  // Specifies passes this one depends on.
  void getAnalysisUsage(AnalysisUsage &Info) const override {
    Info.setPreservesAll();
  }

  const char *getPassName() const override { return "Data Dependence Graph"; }

  void releaseMemory() override { DDG.clear(); }

  // Get the dependence graph.
  const DependenceGraph<Instruction> &getDDG() const { return DDG; }
};
}

#endif
