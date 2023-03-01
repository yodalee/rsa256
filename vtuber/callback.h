#pragma once
#include "abstract_random.h"
#include <functional>
#include <optional>
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
         BoolPattern *new_random_policy_ = nullptr)
      : valid(valid_), ready(ready_), write_func(write_func_) {
    SetRandomValidPolicy(new_random_policy_);
    valid = 0;
  }

  void SetRandomValidPolicy(BoolPattern *new_random_policy_) {
    random_policy.reset(new_random_policy_);
  }

  void before_clk() override {
    // last data has been received, pop it out.
    if (valid && ready) {
      q_source.pop_front();
    }
  }

  bool after_clk() override {
    if (!q_source.empty()) {
      const SC_TYPE &data = q_source.front();
      if (valid == 0 && GetRandom()) {
        valid = true;
        write_func(data);
        return true;
      }
    } else {
      // no data to send.
      // bring down valid, otherwise DUT will get garbage
      const bool prev_valid = valid;
      valid = 0;
      return prev_valid != valid;
    }
    return false;
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
  using NotifyFunc = ::std::function<void(const SC_TYPE &)>;
  Monitor(const CData &valid_, CData &ready_, ReaderFunc read_func_,
          NotifyFunc notify_func_, ::std::function<void()> RaiseFailure_,
          BoolPattern *new_random_policy_ = nullptr)
      : valid(valid_), ready(ready_), read_func(read_func_),
        notify_func(notify_func_), RaiseFailure(RaiseFailure_) {
    SetRandomReadyPolicy(new_random_policy_);
    assert(RaiseFailure);
    ready = 0;
    last_data = std::nullopt;
  }

  void SetRandomReadyPolicy(BoolPattern *new_random_policy_) {
    random_policy.reset(new_random_policy_);
  }

  void before_clk() override {
    if (valid) {
      const SC_TYPE &out = read_func();

      // check output not changed in valid high
      if (!last_data) {
        *last_data = out;
      } else {
        if (out != *last_data) {
          RaiseFailure();
        }
      }

      if (ready) {
        // valid && ready, read data out
        // randomly pull down ready
        notify_func(out);
        last_data.reset();
        ready = !GetRandom();
      }
    } else {
      // last_data will have value if valid has been true
      if (last_data) {
        RaiseFailure();
      }
      last_data.reset();
    }
  }

  bool after_clk() override {
    // If device has pulled up the valid pin, pull up ready pin.
    if (valid && !ready) {
      ready = GetRandom();
      return ready;
    }
    return false;
  }

private:
  const CData &valid;
  CData &ready;
  ::std::optional<SC_TYPE> last_data;
  ::std::unique_ptr<BoolPattern> random_policy;
  ReaderFunc read_func;
  NotifyFunc notify_func;
  ::std::function<void()> RaiseFailure;
  bool GetRandom() { return not random_policy or random_policy->operator()(); }
};
