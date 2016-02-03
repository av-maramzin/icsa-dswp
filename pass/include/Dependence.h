#ifndef ICSA_DSWP_DEPENDENCE
#define ICSA_DSWP_DEPENDENCE

#include <map>
using std::map;
using std::pair;
#include <set>
using std::set;

#include <memory>
using std::unique_ptr;

namespace icsa {

template <typename ValueType> class DependenceGraph {
protected:
  map<const ValueType *, set<const ValueType *>> Nodes;

public:
  typedef typename map<const ValueType *, set<const ValueType *>>::iterator
      nodes_iterator;
  typedef
      typename map<const ValueType *, set<const ValueType *>>::const_iterator
          const_nodes_iterator;

  typedef typename set<const ValueType *>::iterator dependant_iterator;
  typedef
      typename set<const ValueType *>::const_iterator const_dependant_iterator;

  void addNode(const ValueType *Value) { Nodes[Value]; }

  void addEdge(const ValueType *From, const ValueType *To) {
    Nodes[From].insert(To);
  }

  /// Makes sure there are no edges pointing to `Value` and then removes it
  /// from `Nodes`.
  void removeNode(const ValueType *Value) {
    for (auto &Node : Nodes) {
      Node.second.erase(Value);
    }
    Nodes.erase(Value);
  }

  /// Checks if node B depends on node A.
  bool dependsOn(const ValueType *A, const ValueType *B) {
    return Nodes[A].find(B) != Nodes[A].end();
  }

  /// Gets the set of nodes that depend on node A.
  const set<const ValueType *> &getDependants(const ValueType *A) const {
    return Nodes.at(A);
  }

  dependant_iterator child_begin(const ValueType *A) {
    return Nodes.at(A).begin();
  }
  dependant_iterator child_end(const ValueType *A) { return Nodes.at(A).end(); }
  const_dependant_iterator child_cbegin(const ValueType *A) const {
    return Nodes.at(A).cbegin();
  }
  const_dependant_iterator child_cend(const ValueType *A) const {
    return Nodes.at(A).cend();
  }

  /// Remove all nodes from the graph.
  void clear() { Nodes.clear(); }

  nodes_iterator nodes_begin() { return Nodes.begin(); }
  const_nodes_iterator nodes_cbegin() const { return Nodes.cbegin(); }

  nodes_iterator nodes_end() { return Nodes.end(); }
  const_nodes_iterator nodes_cend() const { return Nodes.cend(); }
};
}

#endif
