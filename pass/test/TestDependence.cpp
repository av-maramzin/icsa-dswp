#include <iostream>
using std::cout;

#include <string>
using std::string;

#include "Dependence.h"
using icsa::DependenceGraph;

DependenceGraph<int> initGraph() {
  static int ints[] = {0, 1, 2, 3};

  DependenceGraph<int> G;

  G.addNode(ints + 0);
  G.addNode(ints + 1);
  G.addNode(ints + 2);
  G.addNode(ints + 3);

  G.addEdge(ints + 0, ints + 1);
  G.addEdge(ints + 1, ints + 2);
  G.addEdge(ints + 2, ints + 3);

  G.addEdge(ints + 2, ints + 0);

  return G;
}

void printGraph(const DependenceGraph<int> &G) {
  for (auto I = G.nodes_cbegin(), E = G.nodes_cend(); I != E; ++I) {
    cout << *I->first << ":";
    for (auto J = I->second.cbegin(), F = I->second.cend(); J != F; ++J) {
      cout << " " << **J;
    }
    cout << '\n';
  }
}

int main() {
  DependenceGraph<int> G = initGraph();

  printGraph(G);

  return 0;
}
