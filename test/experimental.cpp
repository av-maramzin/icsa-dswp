#include <cstdlib>

int main(int argc, char *argv[]) {
  if (argc == 3) {
    int x = std::atoi(argv[1]);
    int y = std::atoi(argv[2]);
    return x + y;
  } else {
    return -1;
  }
}
