#ifndef ICSA_DSWP_DEPENDENCE_H
#define ICSA_DSWP_DEPENDENCE_H

#include <memory>
using std::shared_ptr;
#include <array>
using std::array;
#include <vector>
using std::vector;
#include <map>
using std::map;
#include <stdexcept>
using std::out_of_range;
#include <algorithm>
using std::find;

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

  void removeChild(iterator I) { Children.erase(I); }

  bool hasChild(ValueType *Value) {
    for (iterator I = begin(), E = end(); I != E; ++I) {
      if ((*I)->getValue() == Value) {
        return true;
      }
    }
    return false;
  }

  size_t getNumChildren() const { return Children.size(); }

  void clearAllChildren() { Children.clear(); }

  /// Return true if the nodes are not the same and false if they are the same.
  /// Two nodes are the same if they refer to the same value.
  bool compare(const DependenceNode<ValueType> *Other) const {
    return Other->getValue() == getValue();
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

  NodeType *operator[](ValueType *Value) const {
    try {
      return Nodes.at(Value).get();
    } catch (out_of_range &) {
      return nullptr;
    }
  }

  NodeType *operator[](const NodeType &node) const {
    return operator[](node.getValue());
  }

  void addNode(ValueType *Value) {
    Nodes[Value] = shared_ptr<NodeType>(new NodeType(Value));
  }

  void addNode(const NodeType &Node) { addNode(Node.getValue()); }

  // Make sure there aren't any edges pointing to `N` and then remove it
  // from the graph.
  void removeNode(const NodeType &N) {
    for (auto pair : Nodes) {
      NodeType &parent = *(pair.second.get());
      for (auto I = parent.begin(), E = parent.end(); I != E; ++I) {
        if (N.compare(*I)) {
          parent.removeChild(I);
        }
      }
    }

    Nodes.erase(N.getValue());
  }

  NodeType *getNode(ValueType *Value) const { return Nodes.at(Value).get(); }

  typename NodeMapType::const_iterator find(ValueType *Value) const {
    return Nodes.find(Value);
  }

  void addEdge(typename NodeMapType::const_iterator from_it,
               typename NodeMapType::const_iterator to_it) {
    auto from_node = from_it->second.get();
    auto to_node = to_it->second;
    from_node->addChild(to_node);
  }

  void addEdge(ValueType *from, ValueType *to) {
    auto from_it = find(from);
    auto to_it = find(to);
    addEdge(from_it, to_it);
  }

  void addEdge(const NodeType &from, const NodeType &to) {
    addEdge(from.getValue(), to.getValue());
  }

  bool dependsOn(const NodeType &A, const NodeType &B) const {
    return dependsOn(A.getValue(), B.getValue());
  }

  bool dependsOn(ValueType *A, ValueType *B) const {
    auto A_it = find(A);
    return A_it->second.get()->hasChild(B);
  }

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
