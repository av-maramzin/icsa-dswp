#ifndef ICSA_DSWP_PDG_SCC_GRAPH_H
#define ICSA_DSWP_PDG_SCC_GRAPH_H

#include <vector>
using std::vector;

#include <iostream>
using std::cout;

#include "llvm/Support/raw_ostream.h"
using llvm::raw_ostream;
#include "llvm/Support/raw_os_ostream.h"
using llvm::raw_os_ostream;

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

#include "PDG.h"

namespace icsa {

typedef DependenceNode<vector<Instruction *>> PdgSccNode;

class PdgSccGraph : public DependenceGraph<vector<Instruction *>> {
public:
  const Function *getFunction() const {
    Instruction *Inst = (*firstValue)[0];
    raw_os_ostream roos(cout);
    roos << "[DEBUG] ";
    Inst->print(roos);
    roos << '\n';
    return Inst->getParent()->getParent();
  }

  friend class PdgSccGraphPass;
};

// Designed after ControlDependenceGraphPass.
class PdgSccGraphPass : public FunctionPass {
private:
  PdgSccGraph PSG;
  // Keeps track of the memory that needs to be cleaned.
  vector<vector<Instruction *> *> SCCs;

public:
  static char ID;

  PdgSccGraphPass() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override;

  const PdgSccGraph &getPSG() const { return PSG; }

  void getAnalysisUsage(AnalysisUsage &Info) const override {
    Info.setPreservesAll();
    Info.addRequired<ProgramDependenceGraphPass>();
  }

  const char *getPassName() const override {
    return "PDG Strongly Connected Components Graph";
  }

  void releaseMemory() override { 
    PSG.releaseMemory();
    for (auto v : SCCs) {
//      delete v; Just leak it!
    }
  }
};

typedef DependenceBaseIterator<Instruction, PdgSccNode> psg_iterator;
typedef DependenceBaseIterator<Instruction, const PdgSccNode>
    psg_const_iterator;
}

// GraphTraits for PSG.

#include "llvm/ADT/GraphTraits.h"

#include "DependenceGraphTraits.h"

namespace llvm {

template <>
struct GraphTraits<icsa::PdgSccGraph *>
    : public icsa::DGGraphTraits<vector<Instruction *>> {};

template <>
struct GraphTraits<const icsa::PdgSccGraph *>
    : public icsa::ConstDGGraphTraits<vector<Instruction *>> {};
}

#endif
