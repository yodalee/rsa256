#include <functional>
#include <glog/logging.h>
#include <iostream>
#include <queue>

using namespace std;

template <typename DataType> class ScoreBoard {
private:
public:
  ScoreBoard(::std::function<void()> RaiseFailure_)
      : RaiseFailure(RaiseFailure_), pass(true) {
    assert(RaiseFailure);
  };
  ~ScoreBoard() = default;

  void push_golden(const DataType &data) {
    goldens.push_back(data);
    check();
  }
  void push_received(const DataType &data) {
    DLOG(INFO) << "Receive Verilog Out: " << data;
    receiveds.push_back(data);
    check();
  }

  void check() {
    while (goldens.size() != 0 && receiveds.size() != 0) {
      const DataType &received = receiveds.front();
      const DataType &golden = goldens.front();

      if (memcmp(&golden, &received, sizeof(DataType)) == 0) {
      } else {
        LOG(ERROR) << "Golden != Verilog Out: " << golden << " vs " << received;
        pass = false;
        RaiseFailure();
      }
      goldens.pop_front();
      receiveds.pop_front();
    }
  }

  bool is_pass() { return pass && goldens.empty() && receiveds.empty(); }

private:
  ::std::deque<DataType> goldens;
  ::std::deque<DataType> receiveds;
  ::std::function<void()> RaiseFailure;
  bool pass;
};