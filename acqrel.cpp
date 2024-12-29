#include <atomic>
#include <cassert>
#include <iostream>
#include <thread>

std::atomic<bool> x, y;
std::atomic<int> z;

std::atomic<bool> go;

void write_x() {
  while (!go) {
    std::this_thread::yield();
  }
  x.store(true, std::memory_order_release);
}

void write_y() {
  while (!go) {
    std::this_thread::yield();
  }
  y.store(true, std::memory_order_release);
}

void read_x_then_y() {
  while (!go) {
    std::this_thread::yield();
  }
  while (!x.load(std::memory_order_acquire)) {
    // std::cout << "!x.load" << std::endl;
  }
  if (y.load(std::memory_order_acquire)) {
    // std::cout << "y.load" << std::endl;
    ++z;
  }
}

void read_y_then_x() {
  while (!go) {
    std::this_thread::yield();
  }
  while (!y.load(std::memory_order_acquire)) {
    // std::cout << "!y.load" << std::endl;
  }

  if (x.load(std::memory_order_acquire)) {
    ++z;
    // std::cout << "x.load" << std::endl;
  }
}

int main(void) {
  x = false;
  y = false;
  z = 0;

  go = false;

  std::thread a(write_x);
  std::thread b(write_y);
  std::thread c(read_x_then_y);
  std::thread d(read_y_then_x);

  go = true;

  a.join();
  b.join();
  c.join();
  d.join();

  // because both sets of threads independently use acquire and release
  // there is a chance this would hit though it is difficult to reproduce
  // it is 99% 1 or 2 but not guaranteed because both loads could evaluate to
  // false at exactly the same time
  assert(z.load() != 0);
}