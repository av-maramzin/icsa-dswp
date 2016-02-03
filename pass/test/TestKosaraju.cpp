// Ad-hoc test for the Kosajaru algorithm.
#include "GraphUtils.h"

#include "Dependence.h"
#include "DependenceTraits.h"

#include <map>
using std::map;

#include <iostream>
using std::cout;
using std::endl;

struct Foo {
  int id;
};
typedef icsa::DependenceGraph<int> FooGraph;

int main() {
  FooGraph g;
  int n = 8;
  int ints[] = {0, 1, 2, 3, 4, 5, 6, 7};
  for (int &i : ints) {
    g.addNode(&i);
  }

  for (int i = 1; i < n; ++i) {
    g.addEdge(&ints[i - 1], &ints[i]);
  }
  g.addEdge(&ints[1], &ints[0]);
  g.addEdge(&ints[7], &ints[5]);

  map<const int *, const int *> sccs = icsa::GraphUtils<int>::findSCC(g);

  for (const auto &pair : sccs) {
    cout << *pair.first << ": " << *pair.second << endl;
  }

  return 0;
}
