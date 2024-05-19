#pragma once

#include "abstract_random.h"
#include "glog/logging.h"
#include "nosysc.h"

static const int kLIMIT = 10;

template <typename T = unsigned> struct VRDriver {
  VRDriver(BoolPattern *reng_ = nullptr) {
    SetRandomValidPolicy(reng_);
    counter = 0;
  }
  T counter;
  nosysc::ValidReadyOutIf<T> *o;
  ::std::unique_ptr<BoolPattern> reng;

  void ClockedBy(nosysc::Clock &clk) {
    clk.AddIfDependency([this]() { always_comb(); }, {o});
  }

  void SetRandomValidPolicy(BoolPattern *reng_) { reng.reset(reng_); }

  void always_comb() {
    DLOG(INFO) << "Driver comb";
    if (counter < kLIMIT and o->is_writeable() and
        random_factory::GetRandom(reng.get())) {
      o->write(counter);
      LOG(INFO) << "Driver write: " << counter;
      counter++;
    }
  }
};
