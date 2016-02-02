#ifndef ICSA_DSWP_PDG_H
#define ICSA_DSWP_PDG_H

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

#include "CDG.h"
#include "MDG.h"
#include "DDG.h"

namespace icsa {

typedef InstructionDependenceNode ProgramDependenceNode;

class ProgramDependenceGraph : public DependenceGraph<Instruction> {
public:
  const Function *getFunction() const {
    return firstValue->getParent()->getParent();
  }

  friend class ProgramDependenceGraphPass;
};

// Designed after ControlDependenceGraphPass.
class ProgramDependenceGraphPass : public FunctionPass {
private:
  ProgramDependenceGraph PDG;

public:
  static char ID;

  ProgramDependenceGraphPass() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override;

  const ProgramDependenceGraph &getPDG() const { return PDG; }

  void getAnalysisUsage(AnalysisUsage &Info) const override {
    Info.setPreservesAll();
    Info.addRequired<ControlDependenceGraphPass>();
    Info.addRequired<MemoryDependenceGraphPass>();
    Info.addRequired<DataDependenceGraphPass>();
  }

  const char *getPassName() const override {
    return "Program Dependence Graph";
  }

  void releaseMemory() override { PDG.releaseMemory(); }
};

typedef DependenceBaseIterator<Instruction, ProgramDependenceNode> pdg_iterator;
typedef DependenceBaseIterator<Instruction, const ProgramDependenceNode>
    pdg_const_iterator;
}

// GraphTraits for PDG.

#include "llvm/ADT/GraphTraits.h"

#include "DependenceGraphTraits.h"

namespace llvm {

template <>
struct GraphTraits<icsa::ProgramDependenceGraph *>
    : public icsa::DGGraphTraits<Instruction> {};

template <>
struct GraphTraits<const icsa::ProgramDependenceGraph *>
    : public icsa::ConstDGGraphTraits<Instruction> {};
}

#endif
