#include <atomic>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <thread>

std::atomic<bool> x, y;
std::atomic<int> z;

void write_x() {
  x.store(true, std::memory_order_seq_cst);
  std::cout << "x.store" << std::endl;
}

void write_y() {
  y.store(true, std::memory_order_seq_cst);
  std::cout << "y.store" << std::endl;
}

void read_x_then_y() {
  while (!x.load(std::memory_order_seq_cst)) {
    std::cout << "read_x_then_y: !x.load" << std::endl;
  }

  // if this returns false the the store to x must
  // occur before the return to y
  if (y.load(std::memory_order_seq_cst)) {
    std::cout << "read_x_then_y: y.load" << std::endl;
    ++z;
  }
}

void read_y_then_x() {
  while (!y.load(std::memory_order_seq_cst)) {
    std::cout << "read_y_then_x: !y.load" << std::endl;
  }

  if (x.load(std::memory_order_seq_cst)) {
    std::cout << "read_y_then_x: x.load" << std::endl;
    ++z;
  }
}

/**
 *
 *
 * @return int
 */
int main(void) {
  x.store(false);  // default is sequential
  y.store(false);
  z.store(0);

  std::thread a(write_x);
  std::thread b(write_y);
  std::thread c(read_x_then_y);
  std::thread d(read_y_then_x);

  a.join();
  b.join();
  c.join();
  d.join();

  // building with release sometimes 2 and sometimes 1 but never zero
  assert(z.load() != 0);
  std::printf("\n\n%d\n\n", z.load());
}