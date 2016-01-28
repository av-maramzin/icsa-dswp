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
#include "InstDep.h"

namespace icsa {

typedef InstructionDependenceNode DataDependenceNode;

class DataDependenceGraph : public DependenceGraph<Instruction> {
public:
  const Function *getFunction() const {
    return firstValue->getParent()->getParent();
  }

  friend class DataDependenceGraphPass;
};

// Designed after ControlDependenceGraphPass.
class DataDependenceGraphPass : public FunctionPass {
private:
  DataDependenceGraph DDG;

public:
  static char ID;

  DataDependenceGraphPass() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override;

  const DataDependenceGraph &getDDG() const { return DDG; }

  void getAnalysisUsage(AnalysisUsage &Info) const override {
  }

  const char *getPassName() const override {
    return "Data Dependence Graph";
  }

  void print(raw_ostream &OS, const Module *) const override { DDG.print(OS); }

  void releaseMemory() override { DDG.releaseMemory(); }
};

typedef DependenceBaseIterator<Instruction, DataDependenceNode> ddg_iterator;
typedef DependenceBaseIterator<Instruction, const DataDependenceNode>
    ddg_const_iterator;
}

// GraphTraits for DDG.

#include "llvm/ADT/GraphTraits.h"

#include "DependenceGraphTraits.h"

namespace llvm {

template <>
struct GraphTraits<icsa::DataDependenceGraph *>
    : public icsa::DGGraphTraits<Instruction> {};

template <>
struct GraphTraits<const icsa::DataDependenceGraph *>
    : public icsa::ConstDGGraphTraits<Instruction> {};
}

#endif
