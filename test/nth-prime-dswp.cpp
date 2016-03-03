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

template <typename T> class pc_queue {
private:
  std::mutex mutex;                       // used to ensure thread-safety
  std::condition_variable data_available; // used to ensure thread-safety
  std::queue<T> data;

public:
  void produce(T element) {
    std::unique_lock<std::mutex> lock(mutex);
    data.push(element);
    data_available.notify_one();
  }

  T consume() {
    std::unique_lock<std::mutex> lock(mutex);
    if (data.empty()) {
      data_available.wait(lock);
    }
    T element = data.front();
    data.pop();
    return element;
  }
};

const int N = 200000;
uint primes[N];

// Message that tells the consumer to stop.
int game_over = -1;

struct safe_args_t {
  pc_queue<int> &queue;
  safe_args_t(pc_queue<int> &queue)
      : queue(queue) {}
};

void consumer_safe(const safe_args_t &args) {
  uint p = 2;

  while (true) {
    int count = args.queue.consume();
    if (count == game_over) break;
continue_while:
    for (int i = 0; i < count; ++i) {
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
  int count = 0;

  pc_queue<int> queue;
  safe_args_t args(queue);
  std::thread consumer(consumer_safe, args);

  while (count < N) {
    queue.produce(count++);
  }
  queue.produce(game_over);

  consumer.join();

  cout << primes[N - 1] << '\n';
  return 0;
}
