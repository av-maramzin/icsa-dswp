#ifndef ICSA_DSWP_DEPENDENCE_H
#define ICSA_DSWP_DEPENDENCE_H

#include <memory>
using std::shared_ptr;

#include <map>
using std::map;
#include <vector>
using std::vector;
#include "llvm/Support/raw_ostream.h"
using llvm::raw_ostream;

#include "llvm/ADT/SmallPtrSet.h"
using llvm::SmallPtrSet;
#include "llvm/ADT/SmallVector.h"
using llvm::SmallVectorImpl;

#include "llvm/IR/Module.h"
using llvm::Module;
#include "llvm/IR/Function.h"
using llvm::Function;

#include "llvm/Pass.h"
using llvm::FunctionPass;
#include "llvm/PassAnalysisSupport.h"
using llvm::AnalysisUsage;

#include "llvm/Analysis/PostDominators.h"
using llvm::PostDominatorTree;

namespace icsa {

// Designed after DomTreeNodeBase in llvm/Support/GenericDomTree.h.
template <typename ValueType> class Dependence {
  ValueType *TheValue;
  vector<Dependence *> Children;

public:
  typedef typename vector<Dependence *>::iterator iterator;
  typedef typename vector<Dependence *>::const_iterator const_iterator;

  iterator begin() { return Children.begin(); }
  iterator end() { return Children.end(); }
  const_iterator begin() const { return Children.begin(); }
  const_iterator end() const { return Children.end(); }

  ValueType *getValue() const { return TheValue; }
  const vector<Dependence *> &getChildren() const { return Children; }

  Dependence(ValueType *Value) : TheValue(Value) {}

  void addChild(shared_ptr<Dependence> C) { Children.push_back(C.get()); }

  size_t getNumChildren() const { return Children.size(); }

  void clearAllChildren() { Children.clear(); }

  /// Return true if the nodes are not the same and false if they are the same.
  bool compare(const Dependence *Other) const {
    if (getNumChildren() != Other->getNumChildren()) {
      return true;
    }

    SmallPtrSet<const ValueType *, 4> OtherChildren;
    for (const_iterator I = Other->begin(), E = Other->end(); I != E; ++I) {
      const ValueType *Value = (*I)->getValue();
      OtherChildren.insert(Value);
    }

    for (const_iterator I = begin(), E = end(); I != E; ++I) {
      const ValueType *Value = (*I)->getValue();
      if (OtherChildren.count(Value) == 0) {
        return true;
      }
    }
    return false;
  }

  void print(raw_ostream &OS) const {
    getValue()->printAsOperand(OS, false);
    OS << ":";
    for (const_iterator it = begin(); it != end(); ++it) {
      if (it != begin()) {
        OS << ",";
      }
      OS << " ";
      (*it)->getValue()->printAsOperand(OS, false);
    }
  }
};

// Designed after the PostDominatorTree struct in
// llvm/Analysis/PostDominators.h.
template <typename ValueType> class DependenceGraph : public FunctionPass {
protected:
  typedef Dependence<ValueType> DependenceNode;
  typedef map<ValueType *, shared_ptr<DependenceNode>> NodeMapType;
  NodeMapType Nodes;
  ValueType *firstValue;

public:
  DependenceGraph(char ID) : FunctionPass(ID) {}

  DependenceNode *getRootNode() const { return getNode(firstValue); }

  unsigned getNumNodes() const { return Nodes.size(); }

  DependenceNode *operator[](ValueType *Value) const { return getNode(Value); }

  DependenceNode *getNode(ValueType *Value) const {
    typename NodeMapType::const_iterator I = Nodes.find(Value);
    if (I != Nodes.end())
      return I->second.get();
    return nullptr;
  }

  bool dependsOn(DependenceNode *A, DependenceNode *B) const {
    for (typename DependenceNode::const_iterator it = B->begin();
         it != B->end(); ++it) {
      if (false == (*it)->compare(A))
        return true;
    }

    for (typename DependenceNode::const_iterator it = B->begin();
         it != B->end(); ++it) {
      if (dependsOn(A, (*it)))
        return true;
    }

    return false;
  }

  bool dependsOn(ValueType *A, ValueType *B) const {
    DependenceNode *AN = getNode(A), *BN = getNode(B);
    return dependsOn(AN, BN);
  }

  /// Get all nodes that have a control dependence on R.
  void getDependants(ValueType *R, SmallVectorImpl<ValueType *> &Result) const {
    Result.clear();
    const DependenceNode *RN = getNode(R);
    for (typename DependenceNode::const_iterator it = RN->begin();
         it != RN->end(); ++it) {
      Result.push_back((*it)->getValue());
    }
  }

  template <typename T, typename U> friend class DependenceGraphBaseIterator;
};

// Iterator for all DependenceGraph nodes.

// NodeType should be Dependence<ValueType> or const Dependence<ValueType>
template <typename ValueType, typename NodeType>
class DependenceGraphBaseIterator
    : public std::iterator<std::bidirectional_iterator_tag, NodeType, int,
                           NodeType *, NodeType *> {
  typedef std::iterator<std::bidirectional_iterator_tag, NodeType, int,
                        NodeType *, NodeType *> super;
  typedef DependenceGraph<ValueType> GraphType;

public:
  typedef typename super::pointer pointer;
  typedef typename super::reference reference;
  typedef typename GraphType::NodeMapType::const_iterator IteratorType;
  typedef DependenceGraphBaseIterator<ValueType, NodeType> thisType;

private:
  const GraphType &Graph;
  IteratorType idx;

public:
  explicit inline DependenceGraphBaseIterator<ValueType, NodeType>(
      const GraphType &G)
      : Graph(G), idx(G.Nodes.begin()) {}

  inline DependenceGraphBaseIterator<ValueType, NodeType>(const GraphType &G,
                                                          bool)
      : Graph(G), idx(G.Nodes.end()) {}

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
}

#endif
