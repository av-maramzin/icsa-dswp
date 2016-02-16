int main() {
  const int N = 11;
  int array[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};

  for (int i = 0; i < N; ++i) {
    array[i] = array[i] * 2;
  }

  return array[N - 1];
}
