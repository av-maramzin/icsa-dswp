/*
 * Project Euler Problem 3:
 * By listing the first six prime numbers: 2, 3, 5, 7, 11, and 13, we can see
 * that the 6th prime is 13.
 *
 * What is the 10 001st prime number?
 */
#include <iostream>
using std::cout;

#include <cstring>

#include "tinyprof.h"
uint64_t start, total, l1s, l1t;

typedef unsigned int uint;

const uint MAX_PRIME = 20000000;
const uint N = 1000000;
bool sieve[MAX_PRIME];

int main() {
start = rdtsc();
  memset(sieve, true, MAX_PRIME * sizeof(bool));
  sieve[0] = sieve[1] = false;
  uint p = 2;
l1s = rdtsc();
  for (uint count = 1; count < N; ++count) {
    for (uint i = 2; p * i < MAX_PRIME; ++i) {
      sieve[p * i] = false;
    }
    while (!sieve[++p]) ;
  }
l1t = rdtsc() - l1s;
  
  cout << p << '\n';
total = rdtsc() - start;

  cerr << total << ',' << l1t << ',' << (double)l1t / total << '\n';
}
