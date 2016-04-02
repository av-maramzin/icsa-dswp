#include <iostream>
using std::cout;
using std::endl;

#include <string>
using std::string;
using std::stoi;

#include <queue>

#include <pthread.h>

template <typename T> class pc_queue {
private:
  pthread_mutex_t mutex;         // used to ensure thread-safety
  pthread_cond_t data_available; // used to ensure thread-safety
  std::queue<T> data;

public:
  pc_queue() : data() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&data_available, NULL);
  }
  virtual ~pc_queue() {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&data_available);
  }

  void produce(T element) {
    pthread_mutex_lock(&mutex);
    data.push(element);
    pthread_cond_signal(&data_available);
    pthread_mutex_unlock(&mutex);
  }

  T consume() {
    pthread_mutex_lock(&mutex);
    if (data.empty()) {
      pthread_cond_wait(&data_available, &mutex);
    }
    T element = data.front();
    data.pop();
    pthread_mutex_unlock(&mutex);
    return element;
  }
};

bool is_prime(int n) {
  for (int i = 2; i * i <= n; ++i) {
    if (n % i == 0) return false;
  }
  return true;
}

#define MAX_F 10000

long f(long num) {
  long result = 0;
  for (int i = 3; i < MAX_F; ++i) {
    result += is_prime((i * num + 1) % MAX_F);
  }
  return result;
}

long sequential_sum(long N) {
  long sum = 0;

  for (long i = 0; i < N; ++i) {
    sum += f(f(f(f(i))));
  }

  return sum;
}

struct args_t {
  long N;
  pc_queue<long> &queue;
  long &sum;
  args_t(long N, pc_queue<long> &queue, long &sum)
      : N(N), queue(queue), sum(sum) {}
};

void *consumer3(void *args_ptr) {
  args_t args3 = *(args_t *)args_ptr;

  args3.sum = 0;

  for (long i = 0; i < args3.N; ++i) {
    long el = args3.queue.consume();
    args3.sum += f(el);
  }

  return NULL;
}

void *consumer2(void *args_ptr) {
  args_t args2 = *(args_t *)args_ptr;

  pc_queue<long> queue;
  args_t args3(args2.N, queue, args2.sum);

  pthread_t c;
  pthread_create(&c, NULL, consumer3, (void *)&args3);

  for (long i = 0; i < args2.N; ++i) {
    long el = args2.queue.consume();
    queue.produce(f(el));
  }

  pthread_join(c, NULL);

  return NULL;
}

void *consumer1(void *args_ptr) {
  args_t args1 = *(args_t *)args_ptr;

  pc_queue<long> queue;
  args_t args2(args1.N, queue, args1.sum);

  pthread_t c;
  pthread_create(&c, NULL, consumer2, (void *)&args2);

  for (long i = 0; i < args1.N; ++i) {
    long el = args1.queue.consume();
    queue.produce(f(el));
  }

  pthread_join(c, NULL);

  return NULL;
}


long parallel_sum(long N) {
  long sum;
  pc_queue<long> queue;
  args_t args1(N, queue, sum);

  pthread_t c;
  pthread_create(&c, NULL, consumer1, (void *)&args1);

  for (long i = 0; i < N; ++i) {
    queue.produce(f(i));
  }

  pthread_join(c, NULL);

  return sum;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    cout << "usage: " << argv[0] << " <n-elements>" << endl;
    return 0;
  }
  long N = stoi(string(argv[1]));

  long sum = parallel_sum(N);

  cout << "The sum of the first " << N << " numbers is " << sum << endl;

  return 0;
}
