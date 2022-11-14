#pragma once

#include <functional>
#include <iostream>

class Callback {
public:
  virtual ~Callback() = default;
  virtual void before_clk(){};
  virtual bool after_clk() { return false; };
};

template <typename SC_TYPE> class Driver : public Callback {
  using Writer = std::function<void(const SC_TYPE &)>;

public:
  Driver(CData &valid_, const CData &ready_, Writer write_func_)
      : valid(valid_), ready(ready_), write_func(write_func_) {
    valid = 0;
  }

  void before_clk() override {
    // last data has been received, pop it out.
    if (valid == 1 && ready == 1) {
      q_source.pop_front();
    }
    if (!q_source.empty()) {
      const SC_TYPE &data = q_source.front();
      // TODO: probability to present data to DUT
      // This is a always send Driver
      if (valid == 0) {
        valid = 1;
        write_func(data);
      }
    } else {
      // no data to send.
      // bring down valid, otherwise DUT will get garbage
      valid = 0;
    }
  }

  void push_back(const SC_TYPE &data) { this->q_source.push_back(data); }

private:
  CData &valid;
  const CData &ready;
  std::deque<SC_TYPE> q_source;
  Writer write_func;
};

template <typename SC_TYPE> class Monitor : public Callback {
public:
  using Reader = std::function<SC_TYPE()>;
  Monitor(const CData &valid_, CData &ready_, Reader read_func_)
      : valid(valid_), ready(ready_), read_func(read_func_) {
    ready = 1;
  }

  void before_clk() {
    // Always received monitor
    if (valid == 1 && ready == 1) {
      SC_TYPE out = read_func();
      q_destination.push_back(out);
      std::cout << "Receive answer: " << out << std::endl;
    }
  }

private:
  const CData &valid;
  CData &ready;
  std::deque<SC_TYPE> q_destination;
  Reader read_func;
};