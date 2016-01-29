#ifndef ICSA_DSWP_MDG_H
#define ICSA_DSWP_MDG_H

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

#include "llvm/Analysis/MemoryDependenceAnalysis.h"
using llvm::MemoryDependenceAnalysis;

#include "Dependence.h"

namespace icsa {

typedef DependenceNode<Instruction> MemoryDependenceNode;

class MemoryDependenceGraph : public DependenceGraph<Instruction> {
public:
  const Function *getFunction() const {
    return firstValue->getParent()->getParent();
  }

  friend class MemoryDependenceGraphPass;
};

// Designed after ControlDependenceGraphPass.
class MemoryDependenceGraphPass : public FunctionPass {
private:
  MemoryDependenceGraph MDG;

public:
  static char ID;

  MemoryDependenceGraphPass() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override;

  const MemoryDependenceGraph &getMDG() const { return MDG; }

  void getAnalysisUsage(AnalysisUsage &Info) const override {
    Info.setPreservesAll();
    Info.addRequired<MemoryDependenceAnalysis>();
  }

  const char *getPassName() const override {
    return "Memory Dependence Graph";
  }

  void print(raw_ostream &OS, const Module *) const override { MDG.print(OS); }

  void releaseMemory() override { MDG.releaseMemory(); }
};

typedef DependenceBaseIterator<Instruction, MemoryDependenceNode> mdg_iterator;
typedef DependenceBaseIterator<Instruction, const MemoryDependenceNode>
    mdg_const_iterator;
}

// GraphTraits for MDG.

#include "llvm/ADT/GraphTraits.h"

#include "DependenceGraphTraits.h"

namespace llvm {

template <>
struct GraphTraits<icsa::MemoryDependenceGraph *>
    : public icsa::DGGraphTraits<Instruction> {};

template <>
struct GraphTraits<const icsa::MemoryDependenceGraph *>
    : public icsa::ConstDGGraphTraits<Instruction> {};
}

#endif
