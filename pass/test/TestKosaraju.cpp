// Ad-hoc test for the Kosajaru algorithm.
#include "Kosaraju.h"

#include "Dependence.h"
#include "DependenceGraphTraits.h"

#include <map>
using std::map;

#include <iostream>
using std::cout;
using std::endl;

struct Foo {
  int id;
};
typedef icsa::DependenceGraph<Foo> FooGraph;

namespace llvm {

template <> struct GraphTraits<FooGraph *> : public icsa::DGGraphTraits<Foo> {};

template <>
struct GraphTraits<const FooGraph *> : public icsa::ConstDGGraphTraits<Foo> {};
}

int main() {
  FooGraph g;
  int n = 8;
  Foo foos[] = {0, 1, 2, 3, 4, 5, 6, 7};
  for (Foo &foo : foos) {
    g.addNode(&foo);
  }

  for (int i = 1; i < n; ++i) {
    g.addEdge(&foos[i - 1], &foos[i]);
  }
  g.addEdge(&foos[1], &foos[0]);
  g.addEdge(&foos[7], &foos[5]);

  auto sccs = icsa::Kosaraju<FooGraph>::findSCC(g);

  for (const auto &pair : sccs) {
    cout << pair.first->getValue()->id << ':';
    for (const auto &s : pair.second) {
      cout << ' ' << s->getValue()->id;
    }
    cout << endl;
  }

  return 0;
}
