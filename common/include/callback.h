#pragma once
#include "abstract_random.h"
#include <functional>
#include <verilated.h>

class Callback {
public:
  virtual ~Callback() = default;
  virtual void before_clk() {}
  virtual bool after_clk() { return false; }
};

template <typename SC_TYPE> class Driver : public Callback {
public:
  // Write to DUT
  using WriterFunc = ::std::function<void(const SC_TYPE &)>;
  Driver(CData &valid_, const CData &ready_, WriterFunc write_func_,
         BoolPattern *random_policy_)
      : valid(valid_), ready(ready_), write_func(write_func_),
        random_policy(random_policy_) {
    valid = 0;
  }

  void SetRandomValidPolicy(BoolPattern *new_random_policy) {
    random_policy.reset(new_random_policy);
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
  ::std::deque<SC_TYPE> q_source;
  ::std::unique_ptr<BoolPattern> random_policy;
  WriterFunc write_func;
  bool GetRandom() { return not random_policy or random_policy->operator()(); }
};

template <typename SC_TYPE> class Monitor : public Callback {
public:
  // Read from DUT
  using ReaderFunc = ::std::function<SC_TYPE(void)>;
  Monitor(const CData &valid_, CData &ready_, ReaderFunc read_func_,
          BoolPattern *random_policy_)
      : valid(valid_), ready(ready_), read_func(read_func_),
        random_policy(random_policy_) {
    ready = 1;
  }

  void SetRandomReadyPolicy(BoolPattern *new_random_policy) {
    random_policy.reset(new_random_policy);
  }

  void before_clk() {
    // Always received monitor
    if (valid == 1 && ready == 1) {
      SC_TYPE out = read_func();
      q_destination.push_back(out);
    }
  }

private:
  const CData &valid;
  CData &ready;
  ::std::deque<SC_TYPE> q_destination;
  ::std::unique_ptr<BoolPattern> random_policy;
  ReaderFunc read_func;
  bool GetRandom() { return not random_policy or random_policy->operator()(); }
};
