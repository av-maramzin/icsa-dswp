#include <cstdlib>

int main(int argc, char *argv[]) {
  if (argc == 3) {
    int x = std::atoi(argv[1]);
    int y = std::atoi(argv[2]);
    while (x < 0) {
      y = -y; 
      ++x;
    }
    return y;
  } else {
    return -1;
  }
}
