// Implemented in DSWPPass.cpp.
#ifndef ICSA_DSWP_CDG_H
#define ICSA_DSWP_CDG_H

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

namespace llvm {

template <> struct GraphTraits<icsa::ControlDependenceNode *> {
  typedef icsa::ControlDependenceNode NodeType;
  typedef icsa::ControlDependenceNode::iterator ChildIteratorType;

  static NodeType *getEntryNode(icsa::ControlDependenceNode *CDN) {
    return CDN;
  }
  static inline ChildIteratorType child_begin(NodeType *N) {
    return N->begin();
  }
  static inline ChildIteratorType child_end(NodeType *N) { return N->end(); }
};

template <> struct GraphTraits<const icsa::ControlDependenceNode *> {
  typedef const icsa::ControlDependenceNode NodeType;
  typedef icsa::ControlDependenceNode::const_iterator ChildIteratorType;

  static NodeType *getEntryNode(const icsa::ControlDependenceNode *CDN) {
    return CDN;
  }

  static inline ChildIteratorType child_begin(NodeType *N) {
    return N->begin();
  }

  static inline ChildIteratorType child_end(NodeType *N) { return N->end(); }
};

template <>
struct GraphTraits<icsa::ControlDependenceGraph *>
    : public GraphTraits<icsa::ControlDependenceNode *> {
  static NodeType *getEntryNode(icsa::ControlDependenceGraph *CDG) {
    return CDG->getRootNode();
  }

  typedef icsa::cdg_iterator nodes_iterator;

  static nodes_iterator nodes_begin(icsa::ControlDependenceGraph *CDG) {
    return icsa::cdg_iterator(*CDG);
  }
  static nodes_iterator nodes_end(icsa::ControlDependenceGraph *CDG) {
    return icsa::cdg_iterator(*CDG, true);
  }
  static size_t size(icsa::ControlDependenceGraph *CDG) {
    return CDG->getNumNodes();
  }
};

template <>
struct GraphTraits<const icsa::ControlDependenceGraph *>
    : public GraphTraits<const icsa::ControlDependenceNode *> {
  static NodeType *getEntryNode(const icsa::ControlDependenceGraph *CDG) {
    return CDG->getRootNode();
  }

  typedef icsa::cdg_const_iterator nodes_iterator;

  static nodes_iterator nodes_begin(const icsa::ControlDependenceGraph *CDG) {
    return icsa::cdg_const_iterator(*CDG);
  }
  static nodes_iterator nodes_end(const icsa::ControlDependenceGraph *CDG) {
    return icsa::cdg_const_iterator(*CDG, true);
  }
  static size_t size(const icsa::ControlDependenceGraph *CDG) {
    return CDG->getNumNodes();
  }
};
}

#endif
