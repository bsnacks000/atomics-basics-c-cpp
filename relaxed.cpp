#include <atomic>
#include <cassert>
#include <iostream>
#include <thread>

std::atomic<bool> x, y;
std::atomic<int> z;

void write_x_then_y() {
  x.store(true, std::memory_order_relaxed);
  y.store(true, std::memory_order_relaxed);
  // std::cout << "x.store; y.store;" << std::endl;
}

void read_y_then_x() {
  while (!y.load(std::memory_order_relaxed)) {
    // std::cout << "!y.load" << std::endl;
  }

  if (x.load(std::memory_order_relaxed)) {
    // std::cout << "x.load" << std::endl;
  }
}

int main(void) {
  x.store(false);
  y.store(false);
  z.store(0);

  std::thread a(write_x_then_y);
  std::thread b(read_y_then_x);
  a.join();
  b.join();

  // this can fire no ordering is guaranteed between the two threads
  assert(z.load() != 0);
}
