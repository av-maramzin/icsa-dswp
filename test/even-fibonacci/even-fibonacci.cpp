/*
 * Project Euler Problem 2:
 *
 * Each new term in the Fibonacci sequence is generated by adding the previous
 * two terms. By starting with 1 and 2, the first 10 terms will be: 1, 2, 3, 5,
 * 8, 13, 21, 34, 55, 89, ...
 *
 * By considering the terms in the Fibonacci sequence whose values do not exceed
 * four million, find the sum of the even-valued terms.
 */
#include <iostream>
using std::cout;

typedef unsigned int uint;

int main() {
  uint a = 1;
  uint b = 1;
  uint c = a + b;
  uint sum = 0;
  const uint MAX = 4000000;
  while (c <= MAX) {
    if (0 == (c & 1)) sum += c;
    a = b;
    b = c;
    c = a + b;
  }
  cout << sum << '\n';
}
