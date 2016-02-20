typedef struct Node {
  void *payload;
} Node;

/// Applies `f` to the payload of every element of the array.
/// It looks like this function should be parallelisable, but it depends on the
/// `f` function given as input. In the example below, running this in an
/// arbitrary order will give arbitrary results.

void applyToArray(Node nodes[], int nNodes, void (*f)(void *)) {
  for (int i = 0; i < nNodes; ++i) {
    f(nodes[i].payload);
  }
}

void deleteNext(void *payload) {  | void square(void *payload) {
  Node *a = (Node *)payload;      |   int *a = (int *)payload;
  Node *b = (Node *)a->payload;   |   *a = *a * *a;
  Node *c = (Node *)b->payload;   | }
  a->payload = (void *)c;
  b->payload = (void *)b;
}

void square(void *payload) {
  int *a = (int *)payload;
  *a = *a * *a;
}

#include <iostream>
using std::cout;

int main() {
  constexpr int N = 8;
  Node nodes[N];
  for (int i = 1; i < N; ++i) {
    nodes[i - 1].payload = &nodes[i];
  }
  nodes[N - 1].payload = &nodes[0];

  cout << "before\n";
  for (int i = 0; i < N; ++i) {
    cout << &nodes[i] << ": " << nodes[i].payload << "\n";
  }

  applyToArray(nodes, N, deleteNext);

  cout << "after\n";
  for (int i = 0; i < N; ++i) {
    cout << &nodes[i] << ": " << nodes[i].payload << "\n";
  }

  int ints[] = {0, 1, 2, 3, 4, 5, 6, 7};
  for (int i = 0; i < N; ++i) {
    nodes[i].payload = &ints[i];
  }

  cout << "before\n";
  for (int i = 0; i < N; ++i) {
    cout << i << ": " << *(int *)nodes[i].payload << "\n";
  }

  applyToArray(nodes, N, square);

  cout << "after\n";
  for (int i = 0; i < N; ++i) {
    cout << i << ": " << *(int *)nodes[i].payload << "\n";
  }



  return 0;
}
