#ifndef ICSA_DSWP_DEPENDENCE
#define ICSA_DSWP_DEPENDENCE

#include <map>
using std::map;
#include <set>
using std::set;

#include <memory>
using std::unique_ptr;

namespace icsa {

template <typename VT> class DependenceGraph {
protected:
  typedef DependenceGraph<VT> DepGraph;
  map<VT *, set<VT *>> Nodes;

public:
  typedef typename map<VT *, set<VT *>>::iterator nodes_iterator;
  typedef typename map<VT *, set<VT *>>::const_iterator const_nodes_iterator;

  void addNode(VT *Value) { Nodes[Value]; }

  void addEdge(VT *From, VT *To) { Nodes[From].insert(To); }

  /// Makes sure there are no edges pointing to `Value` and then removes it
  /// from `Nodes`.
  void removeNode(VT *Value) {
    for (auto &Node : Nodes) {
      Node.second.erase(Value);
    }
    Nodes.erase(Value);
  }

  nodes_iterator begin() { return Nodes.begin(); }
  const_nodes_iterator cbegin() const { return Nodes.cbegin(); }

  nodes_iterator end() { return Nodes.end(); }
  const_nodes_iterator cend() const { return Nodes.cend(); }
};
}

#endif
