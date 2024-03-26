#pragma once

#include "abstract_random.h"
#include "nosysc.h"
#include <iostream>
#include <memory>
#include <random>

namespace nosysc {

struct Monitor {
  Monitor(BoolPattern *new_reng = nullptr) { SetRandomValidPolicy(new_reng); }
  ::std::unique_ptr<BoolPattern> reng;
  ValidReadyInIf<unsigned> *i;

  void SetRandomValidPolicy(BoolPattern *new_reng) { reng.reset(new_reng); }

  void ClockedBy(Clock &clk) {
    clk.AddIfDependency([this]() { always_comb(); }, {i});
  }

  void always_comb() {
    ::std::cout << "Monitor" << ::std::endl;
    if (i->is_readable() and (not reng or reng->operator()())) {
      ::std::cout << "Read " << i->read() << ::std::endl;
    }
  }
};

} // namespace nosysc