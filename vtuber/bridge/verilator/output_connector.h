#pragma once
#include "abstract_random.h"
#include <functional>
#include <glog/logging.h>
#include <optional>
#include <verilated.h>

template <typename SC_TYPE, typename DUT>
class OutputConnector : public Connector<DUT> {
public:
  // Read from DUT
  using NotifyFunc = ::std::function<void(const SC_TYPE &)>;
  OutputConnector(const CData &valid_, CData &ready_, NotifyFunc notify_func_,
                  ::std::function<void()> RaiseFailure_,
                  BoolPattern *new_random_policy_ = nullptr)
      : valid(valid_), ready(ready_), notify_func(notify_func_),
        RaiseFailure(RaiseFailure_) {
    SetRandomReadyPolicy(new_random_policy_);
    assert(RaiseFailure);
    ready = 0;
    last_data = std::nullopt;
  }

  void SetRandomReadyPolicy(BoolPattern *new_random_policy_) {
    random_policy.reset(new_random_policy_);
  }

  virtual SC_TYPE read(const DUT *dut) = 0;

  void before_clk(DUT *dut) override {
    if (valid) {
      const SC_TYPE &out = read(dut);

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

  bool after_clk(DUT *dut) override {
    // If device has pulled up the valid pin, pull up ready pin.
    if (valid && !ready) {
      ready = GetRandom();
      return ready;
    }
    return false;
  }

  bool is_pass(DUT *dut) override {
    LOG(INFO) << "output_connector final_is_pass";
    return true;
  }

private:
  const CData &valid;
  CData &ready;
  ::std::optional<SC_TYPE> last_data;
  ::std::unique_ptr<BoolPattern> random_policy;
  NotifyFunc notify_func;
  ::std::function<void()> RaiseFailure;
  bool GetRandom() { return not random_policy or random_policy->operator()(); }
};
