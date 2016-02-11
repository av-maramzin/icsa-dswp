/*******************************************************************************
 *
 * (please update README.md if you modify this comment).
 *
 * The starting point for understanding the code is this header file. It
 * implements the template class `DependenceGraph<ValueType>`. This class
 * represents dependences between abstract objects of type `ValueType` - let's
 * call these objects 'nodes' for the sake of discussion. The dependences are
 * represented as a map from nodes to sets of nodes that depend on them.
 * 
 * It is worth noting that nodes are not stored in the Graph, but elsewhere,
 * and are referenced by using pointers to them. For example, if the node
 * `Value` has type `ValueType *` then internally for the `DependenceGraph`
 * class the nodes which depend on `Value` are stored in the set accessed as
 * `Nodes[Value]`. The type of this set is `std::set<ValueType *>`. In other
 * words, the type of `Nodes` is `std::map<ValueType*, std::set<ValueType *>>`.
 * 
 * There is a common sense interface exposed by `DependenceGraph<ValueType>`,
 * allowing users to add and remove nodes, add edges, check if a node depends
 * on another one, and clear the graph. There are also `begin` and `end`
 * iterators which allow the traversal of the nodes of the graph, and are
 * nothing but aliases for the `begin` and `end` iterators of the underlying
 * `map` structure. This allows the user to corrupt the `DependenceGraph`
 * structure, which we hope they won't do.
 * 
 * The type of the node iterators are `std::map<ValueType *, std::set<ValueType
 * *>>::iterator` (aliased to `nodes_iterator`), thus in order to traverse the
 * dependants of a single node, one needs to use `iterator->second.begin()` and
 * `iterator->second.end()`. The type of these iterators is `std::set<ValueType
 * *>::iterator` (aliased to `dependant_iterator`).
 *
*******************************************************************************/

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
