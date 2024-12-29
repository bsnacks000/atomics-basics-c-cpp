#include <atomic>
#include <cassert>
#include <thread>

std::atomic<bool> x, y;
std::atomic<int> z;

void write_x_then_y() {
  // x guaranteed to happen before y since
  // y is syncd with acquire/release semantics
  x.store(true, std::memory_order_relaxed);
  y.store(true, std::memory_order_release);
}

void read_y_then_x() {
  // this synchronizes with the load
  while (!y.load(std::memory_order_acquire))
    ;

  // this will always be true 100% because
  // x.store happened before y.store and y.store is syncd
  if (x.load(std::memory_order_relaxed)) {
    ++z;
  }
}

int main(void) {
  x = false;
  y = false;
  z = 0;

  std::thread a(write_x_then_y);
  std::thread b(read_y_then_x);

  a.join();
  b.join();
  assert(z.load() != 0);
}