#include <iostream>
using std::cout;

#include <string>
using std::string;

#include "NewDep.h"
using icsa::DependenceGraph;
using icsa::Gparasite;
using icsa::Nparasite;

#include "llvm/ADT/GraphTraits.h"
using llvm::GraphTraits;

#include "llvm/Support/raw_ostream.h"
using llvm::errs;
using llvm::raw_fd_ostream;
using llvm::raw_ostream;
#include "llvm/Support/FileSystem.h"
using llvm::sys::fs::F_Text;
#include <system_error>
using std::error_code;

#include "llvm/Support/GraphWriter.h"
using llvm::WriteGraph;


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
  for (auto I = G.cbegin(), E = G.cend(); I != E; ++I) {
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

  typedef GraphTraits<Gparasite<int>> GPTraits;

  Gparasite<int> GP(G);
  for (GPTraits::nodes_iterator I = GPTraits::nodes_begin(GP),
                                E = GPTraits::nodes_end(GP);
       I != E; ++I) {
    Nparasite<int> *N = *I;
    cout << *N->getValue() << ":";
    for (GPTraits::ChildIteratorType J = GPTraits::child_begin(N),
                                     F = GPTraits::child_end(N);
         J != F; ++J) {
      Nparasite<int> *M = *J;
      cout << " " << *M->getValue();
    }
    cout << '\n';
  }

  GP.writeToFile();

  return 0;
}
