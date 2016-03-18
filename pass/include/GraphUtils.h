#ifndef ICSA_DSWP_GRAPH_UTILS_H
#define ICSA_DSWP_GRAPH_UTILS_H

#include <map>
using std::map;
using std::pair;

#include <set>
using std::set;

#include <vector>
using std::vector;

#include <queue>
using std::queue;

#include <algorithm>
using std::find;

#include "Dependence.h"
using icsa::DependenceGraph;

namespace icsa {

template <typename ValueType> class GraphUtils {
public:
  typedef DependenceGraph<ValueType> GraphType;

private:
  // Helper functions.

  // Returns a map from each node of the graph to `false`. Relying on move
  // semantics for efficient return. To be used as a memory of which node is
  // visited.
  static map<const ValueType *, bool> createVisited(const GraphType &g) {
    map<const ValueType *, bool> visited;
    for (auto I = g.nodes_cbegin(), E = g.nodes_cend(); I != E; ++I) {
      visited[I->first] = false;
    }
    return visited;
  }

  // Returns a map from each node to `nullptr`. To be used as a map from nodes
  // to the SCC they belong to.
  static map<const ValueType *, const ValueType *>
  createComponent(const GraphType &g) {
    map<const ValueType *, const ValueType *> component;
    for (auto I = g.nodes_cbegin(), E = g.nodes_cend(); I != E; ++I) {
      component[I->first] = nullptr;
    }
    return component;
  }

  /// u - start node of enumeration
  /// visited - map from nodes to whether they were visited or not
  /// result - a list of the nodes in post-order
  /// g - graph from which to take the nodes;
  static void postOrderEnumerate(const ValueType *u,
                                 map<const ValueType *, bool> &visited,
                                 vector<const ValueType *> &result,
                                 const GraphType &g) {
    if (!visited[u]) {
      visited[u] = true;
      auto deps = g.getDependants(u);
      for (auto I = deps.begin(), E = deps.end(); I != E; ++I) {
        postOrderEnumerate(*I, visited, result, g);
      }
      result.insert(result.begin(), u);
    }
  }

  /// u - current node of recursive traversal
  /// root - current root of traversal
  /// component - store the result here; a map from nodes to their SCC
  /// g - use nodes from this graph as members of `component`
  static void
  assignComponent(const ValueType *u, const ValueType *root,
                  map<const ValueType *, const ValueType *> &component,
                  const GraphType &g) {
    if (component[u] == nullptr) {
      component[u] = root;
      auto deps = g.getDependants(u);
      for (auto I = deps.begin(), E = deps.end(); I != E; ++I) {
        assignComponent(*I, root, component, g);
      }
    }
  }

public:
  // Reverse all edges in `g` and return that as a new graph.
  static GraphType transpose(const GraphType &g) {
    GraphType result;

    for (auto I = g.nodes_cbegin(), E = g.nodes_cend(); I != E; ++I) {
      result.addNode(I->first);
    }

    for (auto I = g.nodes_cbegin(), E = g.nodes_cend(); I != E; ++I) {
      for (auto J = I->second.begin(), F = I->second.end(); J != F; ++J) {
        result.addEdge(*J, I->first);
      }
    }

    return result;
  }

  /// This is an implementation of the Kosaraju algorithm for finding Strongly
  /// Connected Components of a graph. See test/TestKosaraju.cpp for example
  /// usage.
  static map<const ValueType *, const ValueType *> findSCC(const GraphType &g) {
    // Used for the second DFS traversal: assignComponent. The intermediate list
    // indicating the order of the second traversal - postOrder - contains a
    // list of nodes from the `transposedG` graph. When it is used in
    // `assignComponent` the nodes from `g` are used for the final result stored
    // in `component`.
    GraphType transposedG = transpose(g);

    map<const ValueType *, bool> visited = createVisited(g);

    vector<const ValueType *> postOrder;
    for (auto I = transposedG.nodes_cbegin(), E = transposedG.nodes_cend();
         I != E; ++I) {
      postOrderEnumerate(I->first, visited, postOrder, g);
    }

    map<const ValueType *, const ValueType *> component = createComponent(g);
    for (const ValueType *u : postOrder) {
      assignComponent(u, u, component, transposedG);
    }

    return component;
  }

  static vector<const ValueType *> topsort(const GraphType &g) {
    GraphType gt = transpose(g);
    vector<const ValueType *> result;
    queue<const ValueType *> frontier;

    for (auto I = gt.nodes_cbegin(), E = gt.nodes_cend(); I != E; ++I) {
      if (!I->second.empty())
        continue;
      frontier.push(I->first);
    }

    while (!frontier.empty()) {
      auto n = frontier.front();
      frontier.pop();
      result.push_back(n);
      auto deps = g.getDependants(n);
      for (auto I = deps.begin(), E = deps.end(); I != E; ++I) {
        auto m = *I;
        gt.removeEdge(m, n);
        if (gt.getDependants(m).empty()) {
          frontier.push(m);
        }
      }
    }

    return result;
  }
};
}

#endif
