#include <iostream>
using std::cout;

#include <string>
using std::string;

#include "Dependence.h"
using icsa::DependenceGraph;

#include "DependenceTraits.h"
using icsa::DepGraphTraitsWrapper;
using icsa::DepNodeTraitsWrapper;

#include "llvm/ADT/GraphTraits.h"
using llvm::GraphTraits;

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

void printGraphUsingTraits(const DependenceGraph<int> &G) {
  typedef GraphTraits<DepGraphTraitsWrapper<int>> GPTraits;
  DepGraphTraitsWrapper<int> GP(G);
  for (GPTraits::nodes_iterator I = GPTraits::nodes_begin(GP),
                                E = GPTraits::nodes_end(GP);
       I != E; ++I) {
    DepNodeTraitsWrapper<int> *N = *I;
    cout << *N->getValue() << ":";
    for (GPTraits::ChildIteratorType J = GPTraits::child_begin(N),
                                     F = GPTraits::child_end(N);
         J != F; ++J) {
      DepNodeTraitsWrapper<int> *M = *J;
      cout << " " << *M->getValue();
    }
    cout << '\n';
  }
}

void writeGraphUsingTraits(const DependenceGraph<int> &G) {
  DepGraphTraitsWrapper<int>(G).writeToFile("test.dot");
}

int main() {
  DependenceGraph<int> G = initGraph();

  printGraphUsingTraits(G);

  writeGraphUsingTraits(G);

  // Visually inspect the output :) It should be on the standard output and in
  // test.dot.

  return 0;
}
