#pragma once
#include "abstract_random.h"
#include "connector.h"
#include <functional>
#include <optional>
#include <verilated.h>

template <typename SC_TYPE, typename DUT>
class InputConnector : public Connector<DUT> {
public:
  // Write to DUT
  using WriterFunc = ::std::function<void(const SC_TYPE &)>;
  InputConnector(CData &valid_, const CData &ready_, WriterFunc write_func_,
                 BoolPattern *new_random_policy_ = nullptr)
      : valid(valid_), ready(ready_), write_func(write_func_) {
    SetRandomValidPolicy(new_random_policy_);
    valid = 0;
  }

  void SetRandomValidPolicy(BoolPattern *new_random_policy_) {
    random_policy.reset(new_random_policy_);
  }

  void before_clk(DUT *dut) override {
    // last data has been received, pop it out.
    if (valid && ready) {
      q_source.pop_front();
    }
  }

  bool after_clk(DUT *dut) override {
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