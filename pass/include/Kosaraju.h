/// This is an implementation of the Kosaraju algorithm for finding Strongly
/// Connected Components of a graph. The graph should implement
/// llvm::GraphTraits. See test/TestKosaraju.cpp for example usage.
#ifndef ICSA_DSWP_KOSARAJU_H
#define ICSA_DSWP_KOSARAJU_H

#include <map>
using std::map;

#include <vector>
using std::vector;

#include "llvm/ADT/GraphTraits.h"
using llvm::GraphTraits;

namespace icsa {

template <typename GraphType> class Kosaraju {
  typedef GraphTraits<const GraphType *> GTraits;
  typedef typename GTraits::NodeType NodeType;
  typedef typename GTraits::nodes_iterator node_iterator;
  typedef typename GTraits::ChildIteratorType child_iterator;

public:
  static map<NodeType *, vector<NodeType *>> findSCC(const GraphType &g) {
    // Used for the second DFS traversal: assignComponent. The intermediate list
    // indicating the order of the second traversal - postOrder - contains a
    // list of nodes from the `transposedG` graph. When it is used in
    // `assignComponent` the nodes from `g` are used for the final result stored
    // in `component`.
    GraphType transposedG;
    transpose(g, transposedG);

    map<NodeType *, bool> visited = createVisited(g);
    vector<NodeType *> postOrder;
    for (node_iterator I = GTraits::nodes_begin(&g), E = GTraits::nodes_end(&g);
         I != E; ++I) {
      postOrderEnumerate(**I, visited, postOrder, transposedG);
    }

    map<NodeType *, NodeType *> component = createComponent(g);
    for (NodeType *u : postOrder) {
      assignComponent(*u, *u, component, g);
    }

    map<NodeType *, vector<NodeType *>> result;
    for (const auto &pair : component) {
      result[pair.second];
      result[pair.second].push_back(pair.first);
    }

    return result;
  }

private:
  // Returns a map from each node of the graph to `false`. Relying on move
  // semantics for efficient return. To be used as a memory of which node is
  // visited.
  static map<NodeType *, bool> createVisited(const GraphType &g) {
    map<NodeType *, bool> visited;
    for (node_iterator I = GTraits::nodes_begin(&g), E = GTraits::nodes_end(&g);
         I != E; ++I) {
      visited[*I] = false;
    }
    return visited;
  }

  // Returns a map from each node to `nullptr`. To be used as a map from nodes
  // to the SCC they belong to.
  static map<NodeType *, NodeType *> createComponent(const GraphType &g) {
    map<NodeType *, NodeType *> component;
    for (node_iterator I = GTraits::nodes_begin(&g), E = GTraits::nodes_end(&g);
         I != E; ++I) {
      component[*I] = nullptr;
    }
    return component;
  }

  // TODO: implement move constructor and move operator in DependenceGraph and
  // this can just return `result`.
  static void transpose(const GraphType &g, GraphType &result) {
    for (node_iterator I = GTraits::nodes_begin(&g), E = GTraits::nodes_end(&g);
         I != E; ++I) {
      result.addNode(**I);
    }

    for (node_iterator I = GTraits::nodes_begin(&g), E = GTraits::nodes_end(&g);
         I != E; ++I) {
      for (child_iterator J = GTraits::child_begin(*I),
                          F = GTraits::child_end(*I);
           J != F; ++J) {
        result.addEdge(**J, **I);
      }
    }
  }

  /// u - start node of enumeration
  /// visited - map from nodes to whether they were visited or not
  /// result - a list of the nodes in post-order
  /// transposedG - graph from which to take the nodes; this is an artefact of
  ///   the implementation of graphs; there is no way to use the same nodes as
  ///   the forward graph and take their in-neighbours later, if another graph
  ///   is
  ///   not used.
  static void postOrderEnumerate(const NodeType &u,
                                 map<NodeType *, bool> &visited,
                                 vector<NodeType *> &result,
                                 const GraphType &transposedG) {
    if (!visited[&u]) {
      visited[&u] = true;
      for (child_iterator I = GTraits::child_begin(&u),
                          E = GTraits::child_end(&u);
           I != E; ++I) {
        postOrderEnumerate(**I, visited, result, transposedG);
      }
      result.insert(result.begin(), transposedG[u]);
    }
  }

  /// u - current node of recursive traversal
  /// root - current root of traversal
  /// component - store the result here; a map from nodes to their SCC
  /// g - use nodes from this graph as members of `component`
  static void assignComponent(const NodeType &u, const NodeType &root,
                              map<NodeType *, NodeType *> &component,
                              const GraphType &g) {
    if (component[g[u]] == nullptr) {
      component[g[u]] = g[root];
      for (child_iterator I = GTraits::child_begin(&u),
                          E = GTraits::child_end(&u);
           I != E; ++I) {
        assignComponent(**I, root, component, g);
      }
    }
  }
};
}

#endif
