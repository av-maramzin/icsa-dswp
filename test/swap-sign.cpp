// Input: two numbers on the command line
// - if the first one is negative, the sign of the second one is swapped,
//   otherwise the second one is returned unchanged.
// - if there aren't exactly two arguments on the command line, -1 is returned
// - if the arguments cannot be parsed, the program crashes.
#include <cstdlib>

int main(int argc, char *argv[]) {
  if (argc == 3) {
    int x = std::atoi(argv[1]);
    int y = std::atoi(argv[2]);
    if (x < 0) {
      y = -y;
    }
    return y;
  } else {
    return -1;
  }
}
