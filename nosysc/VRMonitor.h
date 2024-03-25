
#pragma once

#include "nosysc.h"
#include <iostream>
#include <random>

namespace nosysc {

::std::bernoulli_distribution dist2(0.5);
::std::default_random_engine reng2;

struct Monitor {
  ValidReadyInIf<unsigned> *i;

  void ClockedBy(Clock &clk) {
    clk.AddIfDependency([this]() { always_comb(); }, {i});
  }

  void always_comb() {
    ::std::cout << "Monitor" << ::std::endl;
    if (i->is_readable() and dist2(reng2)) {
      ::std::cout << "Read " << i->read() << ::std::endl;
    }
  }
};

} // namespace nosysc