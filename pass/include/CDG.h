// Implemented in DSWPPass.cpp.
#ifndef ICSA_DSWP_CDG_H
#define ICSA_DSWP_CDG_H

#include <memory>
using std::shared_ptr;
#include <vector>
using std::vector;
#include <map>
using std::map;
#include <string>
using std::string;

#include "llvm/Support/raw_ostream.h"
using llvm::raw_ostream;

#include "llvm/ADT/SmallVector.h"
using llvm::SmallVectorImpl;
#include "llvm/ADT/SmallPtrSet.h"
using llvm::SmallPtrSet;

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

namespace icsa {

// Designed after DomTreeNodeBase in llvm/Support/GenericDomTree.h.
class ControlDependenceNode {
  BasicBlock *TheBB;
  vector<ControlDependenceNode *> Children;

public:
  typedef typename vector<ControlDependenceNode *>::iterator iterator;
  typedef
      typename vector<ControlDependenceNode *>::const_iterator const_iterator;

  iterator begin() { return Children.begin(); }
  iterator end() { return Children.end(); }
  const_iterator begin() const { return Children.begin(); }
  const_iterator end() const { return Children.end(); }

  BasicBlock *getBlock() const { return TheBB; }
  const vector<ControlDependenceNode *> &getChildren() const {
    return Children;
  }

  ControlDependenceNode(BasicBlock *BB) : TheBB(BB) {}

  void addChild(shared_ptr<ControlDependenceNode> C) {
    Children.push_back(C.get());
  }

  size_t getNumChildren() const { return Children.size(); }

  void clearAllChildren() { Children.clear(); }

  /// Return true if the nodes are not the same and false if they are the same.
  bool compare(const ControlDependenceNode *Other) const {
    if (getNumChildren() != Other->getNumChildren()) {
      return true;
    }

    SmallPtrSet<const BasicBlock *, 4> OtherChildren;
    for (const_iterator I = Other->begin(), E = Other->end(); I != E; ++I) {
      const BasicBlock *BB = (*I)->getBlock();
      OtherChildren.insert(BB);
    }

    for (const_iterator I = begin(), E = end(); I != E; ++I) {
      const BasicBlock *BB = (*I)->getBlock();
      if (OtherChildren.count(BB) == 0) {
        return true;
      }
    }
    return false;
  }

  void print(raw_ostream &OS) const {
    getBlock()->printAsOperand(OS, false);
    OS << ":";
    for (const_iterator it = begin(); it != end(); ++it) {
      if (it != begin()) {
        OS << ",";
      }
      OS << " ";
      (*it)->getBlock()->printAsOperand(OS, false);
    }
  }
};

// Designed after the PostDominatorTree struct in
// llvm/Analysis/PostDominators.h.
class ControlDependenceGraph : public FunctionPass {
private:
  typedef map<BasicBlock *, shared_ptr<ControlDependenceNode>> CDGNodeMapType;
  CDGNodeMapType CDGNodes;
  BasicBlock *firstBB;

public:
  static char ID;

  ControlDependenceGraph() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override;

  void getAnalysisUsage(AnalysisUsage &Info) const override {
    Info.addRequired<PostDominatorTree>();
  }

  const Function *getFunction() const { return firstBB->getParent(); }

  ControlDependenceNode *getRootNode() const { return getNode(firstBB); }

  unsigned getNumNodes() const { return CDGNodes.size(); }

  ControlDependenceNode *operator[](BasicBlock *BB) const;

  ControlDependenceNode *getNode(BasicBlock *BB) const;

  bool dependsOn(ControlDependenceNode *A, ControlDependenceNode *B) const;

  bool dependsOn(BasicBlock *A, BasicBlock *B) const;

  /// Get all nodes that have a control dependence on R.
  void getDependants(BasicBlock *R,
                     SmallVectorImpl<BasicBlock *> &Result) const;

  void releaseMemory() override;

  void print(raw_ostream &OS, const Module *) const override;

  template <typename ValueType> friend class CDGBaseIterator;
};

// Iterator for all CDG nodes.

template <typename ValueType>
class CDGBaseIterator
    : public std::iterator<std::bidirectional_iterator_tag, ValueType, int,
                           ValueType *, ValueType *> {
  typedef std::iterator<std::bidirectional_iterator_tag, ValueType, int,
                        ValueType *, ValueType *> super;

public:
  typedef typename super::pointer pointer;
  typedef typename super::reference reference;
  typedef ControlDependenceGraph::CDGNodeMapType::const_iterator IteratorType;
  typedef CDGBaseIterator<ValueType> thisType;

private:
  const ControlDependenceGraph &Graph;
  IteratorType idx;

public:
  explicit inline CDGBaseIterator<ValueType>(const ControlDependenceGraph &G)
      : Graph(G), idx(G.CDGNodes.begin()) {}

  inline CDGBaseIterator<ValueType>(const ControlDependenceGraph &G, bool)
      : Graph(G), idx(G.CDGNodes.end()) {}

  inline bool operator==(const thisType &other) const {
    return idx == other.idx;
  }

  inline bool operator!=(const thisType &other) const {
    return !operator==(other);
  }

  inline reference operator*() const { return idx->second.get(); }
  inline pointer operator->() const { return operator*(); }

  inline thisType &operator++() {
    ++idx;
    return *this;
  }

  inline thisType operator++(int) {
    thisType tmp = *this;
    ++*this;
    return tmp;
  }

  inline thisType &operator--() {
    --idx;
    return *this;
  }

  inline thisType operator--(int) {
    thisType tmp = *this;
    --*this;
    return tmp;
  }
};

typedef CDGBaseIterator<ControlDependenceNode> cdg_iterator;
typedef CDGBaseIterator<const ControlDependenceNode> cdg_const_iterator;
}

#endif
