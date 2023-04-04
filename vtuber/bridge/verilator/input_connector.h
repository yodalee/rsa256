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
  InputConnector(CData &valid_, const CData &ready_,
                 BoolPattern *new_random_policy_ = nullptr)
      : valid(valid_), ready(ready_) {
    SetRandomValidPolicy(new_random_policy_);
    valid = 0;
    need_write = true;
  }

  void SetRandomValidPolicy(BoolPattern *new_random_policy_) {
    random_policy.reset(new_random_policy_);
  }

  virtual void write(DUT *dut, const SC_TYPE &in) = 0;

  void before_clk(DUT *dut) override {
    // last data has been received, pop it out.
    if (valid && ready) {
      q_source.pop_front();
      need_write = true;
    }
  }

  bool after_clk(DUT *dut) override {
    // no data to send.
    // bring down valid, otherwise DUT will get garbage
    if (q_source.empty() && valid) {
      valid = 0;
      return true;
    }
    // only write data when there are data and new_data need to be set
    if (need_write && !q_source.empty()) {
      const SC_TYPE &data = q_source.front();
      valid = GetRandom();
      if (valid) {
        need_write = false;
        write(dut, data);
        return true;
      }
    }
    return false;
  }

  bool is_pass(DUT *dut) override { return q_source.empty(); }
  void push_back(const SC_TYPE &data) { this->q_source.push_back(data); }

private:
  bool need_write;
  CData &valid;
  const CData &ready;
  ::std::deque<SC_TYPE> q_source;
  ::std::unique_ptr<BoolPattern> random_policy;
  bool GetRandom() { return not random_policy or random_policy->operator()(); }
};