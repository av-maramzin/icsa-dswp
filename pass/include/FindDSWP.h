#ifndef ICSA_DSWP_FIND_DSWP_H
#define ICSA_DSWP_FIND_DSWP_H

#include "llvm/IR/Function.h"
using llvm::Function;

#include "llvm/Pass.h"
using llvm::FunctionPass;
#include "llvm/PassAnalysisSupport.h"
using llvm::AnalysisUsage;

#include "PDG.h"

namespace icsa {

class FindDSWP : public FunctionPass {
public:
  static char ID;

  FindDSWP() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override;

  void getAnalysisUsage(AnalysisUsage &Info) const override {
    Info.setPreservesAll();
    Info.addRequired<ProgramDependenceGraphPass>();
  }
};
}

#endif
