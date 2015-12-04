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

template <typename ValueType, typename NodeType> class DependenceBaseIterator;

// Designed after DomTreeNodeBase in llvm/Support/GenericDomTree.h.
template <typename ValueType> class DependenceNode {
  ValueType *TheValue;
  vector<DependenceNode<ValueType> *> Children;

public:
  typedef vector<DependenceNode<ValueType> *> vector_type;
  typedef typename vector_type::iterator iterator;
  typedef typename vector_type::const_iterator const_iterator;

  iterator begin() { return Children.begin(); }
  iterator end() { return Children.end(); }
  const_iterator begin() const { return Children.begin(); }
  const_iterator end() const { return Children.end(); }

  ValueType *getValue() const { return TheValue; }
  const vector_type &getChildren() const { return Children; }

  DependenceNode<ValueType>(ValueType *Value) : TheValue(Value) {}

  void addChild(shared_ptr<DependenceNode<ValueType>> C) {
    Children.push_back(C.get());
  }

  size_t getNumChildren() const { return Children.size(); }

  void clearAllChildren() { Children.clear(); }

  /// Return true if the nodes are not the same and false if they are the same.
  bool compare(const DependenceNode<ValueType> *Other) const {
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

typedef DependenceNode<BasicBlock> ControlDependenceNode;

// Designed after the PostDominatorTree struct in
// llvm/Analysis/PostDominators.h.
template <typename ValueType> class DependenceGraph {
protected:
  typedef DependenceNode<ValueType> NodeType;
  typedef map<ValueType *, shared_ptr<NodeType>> NodeMapType;
  NodeMapType Nodes;
  ValueType *firstValue;

public:
  NodeType *getRootNode() const { return getNode(firstValue); }

  unsigned getNumNodes() const { return Nodes.size(); }

  NodeType *operator[](ValueType *Value) const;

  NodeType *getNode(ValueType *Value) const;

  bool dependsOn(NodeType *A, NodeType *B) const;

  bool dependsOn(ValueType *A, ValueType *B) const;

  /// Get all nodes that have a control dependence on R.
  void getDependants(ValueType *R, SmallVectorImpl<ValueType *> &Result) const;

  void releaseMemory() {
    firstValue = nullptr;
    Nodes.clear();
  }

  friend class DependenceBaseIterator<ValueType, DependenceNode<ValueType>>;
  friend class DependenceBaseIterator<ValueType,
                                      const DependenceNode<ValueType>>;
};

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

// Iterator for all CDG nodes.

template <typename ValueType, typename NodeType>
class DependenceBaseIterator
    : public std::iterator<std::bidirectional_iterator_tag, NodeType, int,
                           NodeType *, NodeType *> {
  typedef std::iterator<std::bidirectional_iterator_tag, NodeType, int,
                        NodeType *, NodeType *> super;

public:
  typedef typename super::pointer pointer;
  typedef typename super::reference reference;
  typedef typename ControlDependenceGraph::NodeMapType::const_iterator
      NodeMapIterator;

private:
  const ControlDependenceGraph &Graph;
  NodeMapIterator idx;

public:
  // Begin iterator.
  explicit inline DependenceBaseIterator(const ControlDependenceGraph &G)
      : Graph(G), idx(G.Nodes.begin()) {}

  // End iterator.
  inline DependenceBaseIterator(const ControlDependenceGraph &G, bool)
      : Graph(G), idx(G.Nodes.end()) {}

  inline bool operator==(const DependenceBaseIterator &x) const {
    return idx == x.idx;
  }
  inline bool operator!=(const DependenceBaseIterator &x) const {
    return !operator==(x);
  }

  inline reference operator*() const { return idx->second.get(); }
  inline pointer operator->() const { return operator*(); }

  inline DependenceBaseIterator &operator++() {
    ++idx;
    return *this;
  }

  inline DependenceBaseIterator operator++(int) {
    DependenceBaseIterator tmp = *this;
    ++*this;
    return tmp;
  }

  inline DependenceBaseIterator &operator--() {
    --idx;
    return *this;
  }
  inline DependenceBaseIterator operator--(int) {
    DependenceBaseIterator tmp = *this;
    --*this;
    return tmp;
  }
};

typedef DependenceBaseIterator<BasicBlock, ControlDependenceNode> cdg_iterator;
typedef DependenceBaseIterator<BasicBlock, const ControlDependenceNode>
    cdg_const_iterator;
}
using icsa::ControlDependenceNode;
using icsa::ControlDependenceGraph;
using icsa::cdg_iterator;
using icsa::cdg_const_iterator;

// GraphTraits for CDN and CDG.

#include "llvm/ADT/GraphTraits.h"

namespace llvm {

template <> struct GraphTraits<ControlDependenceNode *> {
  typedef ControlDependenceNode NodeType;
  typedef ControlDependenceNode::iterator ChildIteratorType;

  static NodeType *getEntryNode(ControlDependenceNode *CDN) { return CDN; }
  static inline ChildIteratorType child_begin(NodeType *N) {
    return N->begin();
  }
  static inline ChildIteratorType child_end(NodeType *N) { return N->end(); }
};

template <> struct GraphTraits<const ControlDependenceNode *> {
  typedef const ControlDependenceNode NodeType;
  typedef ControlDependenceNode::const_iterator ChildIteratorType;

  static NodeType *getEntryNode(const ControlDependenceNode *CDN) {
    return CDN;
  }

  static inline ChildIteratorType child_begin(NodeType *N) {
    return N->begin();
  }

  static inline ChildIteratorType child_end(NodeType *N) { return N->end(); }
};

template <>
struct GraphTraits<ControlDependenceGraph *>
    : public GraphTraits<ControlDependenceNode *> {
  static NodeType *getEntryNode(ControlDependenceGraph *CDG) {
    return CDG->getRootNode();
  }

  typedef cdg_iterator nodes_iterator;

  static nodes_iterator nodes_begin(ControlDependenceGraph *CDG) {
    return cdg_iterator(*CDG);
  }
  static nodes_iterator nodes_end(ControlDependenceGraph *CDG) {
    return cdg_iterator(*CDG, true);
  }
  static size_t size(ControlDependenceGraph *CDG) { return CDG->getNumNodes(); }
};

template <>
struct GraphTraits<const ControlDependenceGraph *>
    : public GraphTraits<const ControlDependenceNode *> {
  static NodeType *getEntryNode(const ControlDependenceGraph *CDG) {
    return CDG->getRootNode();
  }

  typedef cdg_const_iterator nodes_iterator;

  static nodes_iterator nodes_begin(const ControlDependenceGraph *CDG) {
    return cdg_const_iterator(*CDG);
  }
  static nodes_iterator nodes_end(const ControlDependenceGraph *CDG) {
    return cdg_const_iterator(*CDG, true);
  }
  static size_t size(const ControlDependenceGraph *CDG) {
    return CDG->getNumNodes();
  }
};
}

#endif
