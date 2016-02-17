/*
 * Project Euler Problem 3:
 * By listing the first six prime numbers: 2, 3, 5, 7, 11, and 13, we can see
 * that the 6th prime is 13.
 *
 * What is the 10 001st prime number?
 */
#include <iostream>
using std::cout;

typedef unsigned int uint;

const uint N = 1000000;
uint primes[N];

int main() {
  uint count = 0;
  uint p = 2;
  while (count < N) {
continue_while:
    for (uint i = 0; i < count; ++i) {
      if (0 == p % primes[i]) {
        ++p;
        goto continue_while;
      }
      if (primes[i] * primes[i] > p) {
        break;
      }
    }
    primes[count++] = p++;
  }
  cout << primes[N - 1] << '\n';
  return 0;
}
