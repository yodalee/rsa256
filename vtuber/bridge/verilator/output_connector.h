#pragma once
#include "abstract_random.h"
#include <functional>
#include <optional>
#include <verilated.h>

template <typename SC_TYPE> class OutputConnector : public Connector {
public:
  // Read from DUT
  using ReaderFunc = ::std::function<SC_TYPE(void)>;
  using NotifyFunc = ::std::function<void(const SC_TYPE &)>;
  OutputConnector(const CData &valid_, CData &ready_, ReaderFunc read_func_,
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
