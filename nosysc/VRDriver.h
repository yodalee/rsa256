#pragma once

#include "abstract_random.h"
#include "nosysc.h"
#include <iostream>
#include <memory>
#include <random>

namespace nosysc {

struct Driver {
  Driver(BoolPattern *new_reng = nullptr) { SetRandomValidPolicy(new_reng); }
  ::std::unique_ptr<BoolPattern> reng;
  ValidReadyOutIf<unsigned> *o;
  unsigned counter = 0;

  void SetRandomValidPolicy(BoolPattern *new_reng) { reng.reset(new_reng); }

  void ClockedBy(Clock &clk) {
    clk.AddIfDependency([this]() { always_comb(); }, {o});
  }

  void always_comb() {
    ::std::cout << "Driver" << ::std::endl;
    if (o->is_writeable() and (not reng or reng->operator()())) {
      o->write(counter);
      ::std::cout << "Driver write" << ::std::endl;
      counter++;
    }
  }
};

} // namespace nosysc