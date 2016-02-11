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
  map<ValueType *, set<ValueType *>> Nodes;

public:
  typedef typename map<ValueType *, set<ValueType *>>::iterator nodes_iterator;
  typedef typename map<ValueType *, set<ValueType *>>::const_iterator
      const_nodes_iterator;

  typedef typename set<ValueType *>::iterator dependant_iterator;
  typedef typename set<ValueType *>::const_iterator const_dependant_iterator;

  void addNode(ValueType *Value) { Nodes[Value]; }

  void addEdge(ValueType *From, ValueType *To) { Nodes[From].insert(To); }

  /// Makes sure there are no edges pointing to `Value` and then removes it
  /// from `Nodes`.
  void removeNode(ValueType *Value) {
    for (auto &Node : Nodes) {
      Node.second.erase(Value);
    }
    Nodes.erase(Value);
  }

  /// Remove all nodes from the graph.
  void clear() { Nodes.clear(); }

  nodes_iterator begin() { return Nodes.begin(); }
  const_nodes_iterator cbegin() const { return Nodes.cbegin(); }

  nodes_iterator end() { return Nodes.end(); }
  const_nodes_iterator cend() const { return Nodes.cend(); }
};
}

#endif
