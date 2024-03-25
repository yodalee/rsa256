#pragma once

#include "nosysc.h"
#include <iostream>
#include <random>

namespace nosysc {

::std::bernoulli_distribution dist1(0.5);
::std::default_random_engine reng1;

struct Driver {
  ValidReadyOutIf<unsigned> *o;
  unsigned counter = 0;

  void ClockedBy(Clock &clk) {
    clk.AddIfDependency([this]() { always_comb(); }, {o});
  }

  void always_comb() {
    ::std::cout << "Driver" << ::std::endl;
    if (o->is_writeable() and dist1(reng1)) {
      o->write(counter);
      ::std::cout << "Driver write" << ::std::endl;
      counter++;
    }
  }
};

} // namespace nosysc