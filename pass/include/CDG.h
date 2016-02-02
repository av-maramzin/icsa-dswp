#ifndef ICSA_DSWP_CDG_H
#define ICSA_DSWP_CDG_H

#include "llvm/Support/raw_ostream.h"
using llvm::raw_ostream;

#include "llvm/IR/Module.h"
using llvm::Module;
#include "llvm/IR/Function.h"
using llvm::Function;
#include "llvm/IR/BasicBlock.h"
using llvm::BasicBlock;

#include "llvm/Pass.h"
using llvm::FunctionPass;
#include "llvm/PassAnalysisSupport.h"
using llvm::AnalysisUsage;

#include "llvm/Analysis/PostDominators.h"
using llvm::PostDominatorTree;

#include "Dependence.h"

namespace icsa {

typedef DependenceNode<BasicBlock> ControlDependenceNode;

class ControlDependenceGraph : public DependenceGraph<BasicBlock> {
public:
  const Function *getFunction() const { return firstValue->getParent(); }

  friend class ControlDependenceGraphPass;
};

// Designed after the PostDominatorTree struct in
// llvm/Analysis/PostDominators.h.
class ControlDependenceGraphPass : public FunctionPass {
private:
  ControlDependenceGraph CDG;

public:
  static char ID;

  ControlDependenceGraphPass() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override;

  const ControlDependenceGraph &getCDG() const { return CDG; }

  void getAnalysisUsage(AnalysisUsage &Info) const override {
    Info.setPreservesAll();
    Info.addRequired<PostDominatorTree>();
  }

  const char *getPassName() const override {
    return "Control Dependence Graph";
  }

  void releaseMemory() override { CDG.releaseMemory(); }
};

typedef DependenceBaseIterator<BasicBlock, ControlDependenceNode> cdg_iterator;
typedef DependenceBaseIterator<BasicBlock, const ControlDependenceNode>
    cdg_const_iterator;
}

// GraphTraits for CDN and CDG.

#include "llvm/ADT/GraphTraits.h"

#include "DependenceGraphTraits.h"

namespace llvm {

template <>
struct GraphTraits<icsa::ControlDependenceNode *>
    : public icsa::DNGraphTraits<BasicBlock> {};

template <>
struct GraphTraits<const icsa::ControlDependenceNode *>
    : public icsa::ConstDNGraphTraits<BasicBlock> {};

template <>
struct GraphTraits<icsa::ControlDependenceGraph *>
    : public icsa::DGGraphTraits<BasicBlock> {};

template <>
struct GraphTraits<const icsa::ControlDependenceGraph *>
    : public icsa::ConstDGGraphTraits<BasicBlock> {};
}

#endif
