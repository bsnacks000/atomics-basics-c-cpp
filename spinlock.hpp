#ifndef __LF_SPINLOCK_HPP__
#define __LF_SPINLOCK_HPP__

#include <atomic>

class spinlock_mutex {
 public:
  explicit spinlock_mutex() : flag_(ATOMIC_FLAG_INIT) {}

  void lock() {
    // spin until old value is false, meaning this thread called clear
    while (flag_.test_and_set(std::memory_order_acquire));
  }

  void unlock() { flag_.clear(std::memory_order_release); }

 private:
  std::atomic_flag flag_;
};

#endif
