#include <chrono>
#include <cstdlib>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "ringbuf.hpp"
#include "ttas_spinlock.hpp"

#define MESSAGE_COUNT 100

template <typename T, size_t N = 1024>
class rt_channel {
 public:
  bool write(T* ptr) {
    std::scoped_lock<ttas_spinlock> lg(mut_);
    return buf_.write(ptr);
  }

  bool read(T** ptr) {
    std::scoped_lock<ttas_spinlock> lg(mut_);
    return buf_.read(ptr);
  }

  bool try_read(T** ptr) {
    if (!mut_.try_lock()) {
      return false;  // missed the lock...
    }
    auto result = buf_.read(ptr);
    mut_.unlock();
    return result;
  }

 private:
  ringbuf<T, N> buf_;
  ttas_spinlock mut_;
};

struct write_buffer {
  std::vector<std::string>* message_buf;
  rt_channel<std::string>* chan;
};

void reader(rt_channel<std::string>* chan) {
  auto count = 0;

  // simulate reading from an rt thread...
  bool result;
  std::string* s;

  // artificially exit after we get all the messages
  // try read would prob need to be used in realtime context since we would
  // not really want to block though if there is not alot of contention
  // and the critical section is fast (which is the case with the ringbuf)
  while (count < MESSAGE_COUNT) {
    result = chan->try_read(&s);
    if (result) {
      std::cout << "Reader: " << *s << std::endl;
      count += 1;
    } else {
      std::cout << "Missed!" << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  }
}

void writer(write_buffer* wb) {
  // sleep within this bound...
  int lb = 10, ub = 20;

  for (auto& message : *wb->message_buf) {
    wb->chan->write(&message);
    auto interval = rand() % (ub - lb + 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(interval));
  }
}

int main(void) {
  std::vector<std::string> message_buffer;

  for (int i = 0; i < MESSAGE_COUNT; i++) {
    auto s = std::string{"hi: "};
    s.append(std::to_string(i));
    message_buffer.push_back(s);
  }

  rt_channel<std::string> chan;

  write_buffer wbuf;
  wbuf.chan = &chan;
  wbuf.message_buf = &message_buffer;

  auto w = std::thread(writer, &wbuf);
  auto r = std::thread(reader, &chan);

  w.join();
  r.join();
}
