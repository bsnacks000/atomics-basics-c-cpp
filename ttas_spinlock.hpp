#ifndef __TTAS_SPINLOCK_HPP__
#define __TTAS_SPINLOCK_HPP__

// based on https://www.youtube.com/watch?v=zrWYJ6FdOFQ
// https://geidav.wordpress.com/tag/contention/
// https://www.youtube.com/watch?v=rmGJc9PXpuE
// https://rigtorp.se/spinlock/

#include <immintrin.h>  // would need to also include ARM instructrions since this is for intel.
#include <atomic>
#include <new>

#ifdef __cpp_lib_hardware_interference_size
using std::hardware_constructive_interference_size;
using std::hardware_destructive_interference_size;
#else
// 64 bytes on x86-64 │ L1_CACHE_BYTES │ L1_CACHE_SHIFT │ __cacheline_aligned │
// ...
constexpr std::size_t hardware_constructive_interference_size = 64;
constexpr std::size_t hardware_destructive_interference_size = 64;
#endif

#include <iostream>

// this will spin with exponential backoff

class ttas_spinlock {
 public:
  void lock() noexcept {
    for (;;) {
      if (try_lock()) {
        break;
      }
      int count = 0;
      while (flag_.load(std::memory_order_relaxed)) {
        _mm_pause();
        count++;
        if (count > 16) {
          _mm_pause();
          _mm_pause();
          _mm_pause();
        }
        if (count > 1000) {
          _mm_pause();
          _mm_pause();
          _mm_pause();
          _mm_pause();
          _mm_pause();
          _mm_pause();
        }
      }
    }
  }

  bool try_lock() noexcept {
    return !flag_.exchange(true, std::memory_order_acquire);
  }

  void unlock() noexcept { flag_.store(false, std::memory_order_release); }

 private:
  static_assert(std::atomic<bool>::is_always_lock_free);
  alignas(hardware_destructive_interference_size) std::atomic<bool> flag_ =
      false;
};

#endif

// the pikus impl works
// for (int i = 0; flag_.load(std::memory_order_relaxed) ||
//                     flag_.exchange(true, std::memory_order_acquire);
//          i++) {
//       if (i == 8) {
//         i = 0;
//         _mm_pause();
//         _mm_pause();
//         _mm_pause();
//       }
//     }

// for (int i = 0; i < 5; i++) {
//         if (!flag_.load(std::memory_order_relaxed))
//           return;
//       }

//       for (int i = 0; i < 10; i++) {
//         if (!flag_.load(std::memory_order_relaxed))
//           return;
//         _mm_pause();
//       }

//       while (true) {
//         for (int i = 0; i < 1000; i++) {
//           if (!flag_.load(std::memory_order_relaxed))
//             return;
//           _mm_pause();
//           _mm_pause();
//           _mm_pause();
//           _mm_pause();
//           _mm_pause();
//           _mm_pause();
//           _mm_pause();
//           _mm_pause();
//           _mm_pause();
//           _mm_pause();
//         }
//       }