#pragma once

#include <functional>

#include "abstract_random.h"
#include "glog/logging.h"
#include "nosysc.h"

template <typename T = unsigned> struct VRMonitor {
  using Callback = std::function<void(unsigned)>;
  Callback receive;
  nosysc::ValidReadyInIf<T> *i;
  ::std::unique_ptr<BoolPattern> reng;

  VRMonitor(Callback f, BoolPattern *reng_ = nullptr) : receive(f) {
    SetRandomValidPolicy(reng_);
  }

  void ClockedBy(nosysc::Clock &clk) {
    clk.AddIfDependency([this]() { always_comb(); }, {i});
  }

  void SetRandomValidPolicy(BoolPattern *reng_) { reng.reset(reng_); }

  void always_comb() {
    DLOG(INFO) << "Monitor Comb";
    if (i->is_readable() and random_factory::GetRandom(reng.get())) {
      unsigned d = i->read();
      LOG(INFO) << "Monitor Read: " << d;
      receive(d);
    }
  }
};
