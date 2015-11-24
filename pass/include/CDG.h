// Implemented in DSWPPass.cpp.
#ifndef CDGPASS_H
#define CDGPASS_H

#include "llvm/IR/Function.h"
using llvm::Function;
#include "llvm/IR/Module.h"
using llvm::Module;

#include "llvm/Pass.h"
using llvm::FunctionPass;
#include "llvm/PassAnalysisSupport.h"
using llvm::AnalysisUsage;

class ControlDependenceGraph : public FunctionPass {
public:
  static char ID;
  ControlDependenceGraph();
  void getAnalysisUsage(AnalysisUsage &Info) const override;
  bool runOnFunction(Function &F) override;
};

#endif
