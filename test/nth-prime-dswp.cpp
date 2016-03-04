/*
 * Project Euler Problem 3:
 * By listing the first six prime numbers: 2, 3, 5, 7, 11, and 13, we can see
 * that the 6th prime is 13.
 *
 * What is the 10 001st prime number?
 */
#include <iostream>
using std::cout;

#include <queue>

#include <thread>
#include <mutex>
#include <condition_variable>

typedef unsigned int uint;

// T should have a default constructor: the end-of-queue package will attempt
// to use it.
template <typename T> struct pc_packet {
  bool end_of_queue;
  T datum;  // invalid if end_of_queue is `true`
};

template <typename T> class pc_queue {
private:
  std::mutex mutex;
  std::condition_variable data_available;
  std::queue<pc_packet<T>> data;

public:
  void produce(T element) {
    std::unique_lock<std::mutex> lock(mutex);
    data.push({false, element});
    data_available.notify_one();
  }

  void produce_end() {
    std::unique_lock<std::mutex> lock(mutex);
    data.push({true, T()});
  }

  T consume() {
    std::unique_lock<std::mutex> lock(mutex);
    if (data.empty()) {
      data_available.wait(lock);
    }
    T element = data.front().datum;
    data.pop();
    return element;
  }

  bool end_reached() {
    std::unique_lock<std::mutex> lock(mutex);
    return data.front().end_of_queue;
  }
};

const int N = 200000;
uint primes[N];

// Cannot pass a modifiable reference to a thread function.
void consumer(pc_queue<uint> *const queue) {
  uint p = 2;

  while (!queue->end_reached()) {
    uint count = queue->consume();
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
    primes[count] = p++;
  }
}

int main() {
  uint count = 0;

  pc_queue<uint> queue;
  std::thread c(consumer, &queue);

  while (count < N) {
    queue.produce(count++);
  }
  queue.produce_end();

  c.join();

  cout << primes[N - 1] << '\n';
  return 0;
}
