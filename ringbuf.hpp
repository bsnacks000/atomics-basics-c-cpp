#ifndef __RINGBUF_HPP__
#define __RINGBUF_HPP__

#include <array>
#include <atomic>

template <typename T, size_t N = 1024>
class ringbuf {
 public:
  bool write(T* ptr) {
    if (!is_full()) {
      arr_[w_ % N] = ptr;
      w_++;
      return true;
    }
    return false;
  }

  bool read(T** ptr) {
    if (!is_empty()) {
      *ptr = arr_[r_ % N];
      r_++;
      return true;
    }
    return false;
  }

 private:
  inline bool is_empty() { return w_ - r_ == 0; }
  inline bool is_full() { return N - w_ - r_ == 0; }

  std::array<T*, N> arr_;
  int64_t r_ = 0;
  int64_t w_ = 0;
};

#endif
