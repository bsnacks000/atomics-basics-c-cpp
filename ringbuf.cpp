#include <array>
#include <atomic>
#include <cassert>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

// single threaded ringbuf

// slots = (N - (write_ptr index + 1)) + read_ptr index

// [0][1][2][3][4][5][6][7][8][9]
// [-][-][r*][*][*][w-][-][-][-][-]
// N = 10
// (10 - (5 + 1)) + 2 = 4 right hand slots
//

// https://sites.google.com/view/algobytheroyakash/data-structures/ring-buffer
// its easier/faster to use an int64 constantly incrementing and then do modulo
// idx rather then loop

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

struct obj {
  int n;
};

void print_obj(obj* o) {
  if (o == nullptr) {
    throw "nullptr";
  }
  std::cout << "{" << o->n << "}\n";
}

int main(void) {
  ringbuf<obj, 10> buf;

  std::vector<obj> objects;

  for (int i = 0; i < 20; i++) {
    auto o = obj{.n = i};
    objects.push_back(o);
  }

  for (auto& o : objects) {
    bool res = buf.write(&o);
    if (!res) {
      std::cout << "No room: " << o.n << "\n";
    }
  }

  int counter = 0;
  for (int i = 0; i < 20; i++) {
    obj* o;
    bool res = buf.read(&o);
    if (!res) {
      counter++;
      std::cout << "Empty buffer: " << counter << "\n";
    }
  }

  for (int i = 0; i < 100; i++) {
    bool res = buf.write(&objects[0]);
    if (!res)
      throw "buf.write false";

    obj* o;
    res = buf.read(&o);
    if (!res)
      throw "buf.read false";
  }
}