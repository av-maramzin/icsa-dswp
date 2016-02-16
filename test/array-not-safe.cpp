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

void swap(void *node) {
  Node *n = (Node *)node;
  Node *next = (Node *)n->payload;
  n->payload = (void *)next->payload;
  next->payload = (void *)n;
}

int main() {
  constexpr int N = 16;
  Node nodes[N];
  for (int i = 1; i < N; ++i) {
    nodes[i - 1].payload = &nodes[i];
  }
  nodes[N - 1].payload = &nodes[0];

  applyToArray(nodes, 0, swap);
  return 0;
}
