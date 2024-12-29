#include "ttas_spinlock.hpp"

#include <iostream>
#include <thread>
#include <vector>

// int64_t counter = 0;

struct data {
  int64_t counter = 0;
  ttas_spinlock lock;
};

void worker(data* d) {
  int count = 0;
  for (int i = 0; i < 1000000; i++) {
    d->lock.lock();
    d->counter++;
    d->lock.unlock();
    count++;
  }
  std::cout << "worker exit: " << count << std::endl;
}

int main(void) {
  auto d = data();

  auto t1 = std::thread(worker, &d);
  auto t2 = std::thread(worker, &d);
  auto t3 = std::thread(worker, &d);
  auto t4 = std::thread(worker, &d);

  t1.join();
  t2.join();
  t3.join();
  t4.join();

  std::cout << "counter: " << d.counter << std::endl;
}