#ifndef ICSA_DSWP_DEPENDENCE_H
#define ICSA_DSWP_DEPENDENCE_H

#include <memory>
using std::shared_ptr;
#include <vector>
using std::vector;
#include <map>
using std::map;

#include "llvm/Support/raw_ostream.h"
using llvm::raw_ostream;

#include "llvm/ADT/SmallVector.h"
using llvm::SmallVectorImpl;
#include "llvm/ADT/SmallPtrSet.h"
using llvm::SmallPtrSet;

namespace icsa {

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

// Forward declaration for friends in DependenceGraph.
template <typename ValueType, typename NodeType> class DependenceBaseIterator;

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

  void addNode(ValueType *Value) {
    Nodes[Value] = shared_ptr<NodeType>(new NodeType(Value));
  }

  NodeType *getNode(ValueType *Value) const;

  typename NodeMapType::iterator find(ValueType *Value) {
    return Nodes.find(Value);
  }

  void addEdge(typename NodeMapType::iterator from_it,
               typename NodeMapType::iterator to_it) {
    auto from_node = from_it->second.get();
    auto to_node = to_it->second;
    from_node->addChild(to_node);
  }

  void addEdge(ValueType *from, ValueType *to) {
    auto from_it = find(from);
    auto to_it = find(to);
    addEdge(from_it, to_it);
  }

  bool dependsOn(NodeType *A, NodeType *B) const;

  bool dependsOn(ValueType *A, ValueType *B) const;

  /// Get all nodes that have a dependence on R.
  vector<ValueType *> getDependants(ValueType *R) const {
    NodeType *N = getNode(R);
    vector<ValueType *> result;
    for (auto it = N->begin(); it != N->end(); ++it) {
      ValueType *ptr = (*it)->getValue();
      result.push_back(ptr);
    }
    return result;
  }

  void releaseMemory() {
    firstValue = nullptr;
    Nodes.clear();
  }

  void print(raw_ostream &OS) const {
    OS << "=============================--------------------------------\n";
    OS << "Dependence Graph: ";
    OS << "<node: dependants>";
    OS << "\n";
    for (typename NodeMapType::const_iterator I = Nodes.begin();
         I != Nodes.end(); ++I) {
      I->second.get()->print(OS);
      OS << '\n';
    }
  }

  friend class DependenceBaseIterator<ValueType, DependenceNode<ValueType>>;
  friend class DependenceBaseIterator<ValueType,
                                      const DependenceNode<ValueType>>;
};

// Iterator for all Dependence nodes.
template <typename ValueType, typename NodeType>
class DependenceBaseIterator
    : public std::iterator<std::bidirectional_iterator_tag, NodeType, int,
                           NodeType *, NodeType *> {
  typedef std::iterator<std::bidirectional_iterator_tag, NodeType, int,
                        NodeType *, NodeType *> super;

public:
  typedef DependenceGraph<ValueType> GraphType;
  typedef typename super::pointer pointer;
  typedef typename super::reference reference;
  typedef typename GraphType::NodeMapType::const_iterator NodeMapIterator;

private:
  const DependenceGraph<ValueType> &Graph;
  NodeMapIterator idx;

public:
  // Begin iterator.
  explicit inline DependenceBaseIterator(const GraphType &G)
      : Graph(G), idx(G.Nodes.begin()) {}

  // End iterator.
  inline DependenceBaseIterator(const GraphType &G, bool)
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
}

#endif
