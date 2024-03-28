#include "VDut.h"
#include "nosysc.h"
#include "verilated_fst_c.h"

#include <functional>
#include <glog/logging.h>
#include <iostream>
#include <memory>
#include <queue>
#include <random>
#include <type_traits>
#include <utility>

using namespace std;
using namespace nosysc;

// User code
bernoulli_distribution dist(0.5);
default_random_engine reng;

// Driver and Monitor is reused
struct Driver {
  ValidReadyOutIf<unsigned> *o;
  unsigned counter = 0;

  void ClockedBy(Clock &clk) {
    clk.AddIfDependency([this]() { always_comb(); }, {o});
  }

  void always_comb() {
    cout << "Driver" << endl;
    if (o->is_writeable() and dist(reng)) {
      o->write(counter);
      cout << "Driver write" << endl;
      counter++;
    }
  }
};

struct Monitor {
  ValidReadyInIf<unsigned> *i;

  void ClockedBy(Clock &clk) {
    clk.AddIfDependency([this]() { always_comb(); }, {i});
  }

  void always_comb() {
    cout << "Monitor" << endl;
    if (i->is_readable() and dist(reng)) {
      cout << "Read " << i->read() << endl;
    }
  }
};

// V1: c-model
struct Dut {
  ValidReadyInIf<unsigned> *i;
  ValidReadyOutIf<unsigned> *o;

  void ClockedBy(Clock &clk) {
    clk.AddIfDependency([this]() { always_comb(); }, {i, o});
  }

  void always_comb() {
    if (o->is_writeable() and i->is_readable()) {
      o->write(i->read());
    }
  }
};

// V2: Verilated
class DutWrapper {
  VerilatedContext contextp;
  VerilatedFstC tfp;
  VDut vdut;
  unsigned counter = 0;

public:
  ValidReadyInIf<unsigned> *i;
  ValidReadyOutIf<unsigned> *o;

  DutWrapper() {
    Verilated::traceEverOn(true);
    vdut.trace(&tfp, 99);
    tfp.open("Dut.fst");

    vdut.clk = 0;
    vdut.rst_n = 1;
    vdut.eval();
    tfp.dump(counter++);

    vdut.rst_n = 0;
    vdut.eval();
    tfp.dump(counter++);

    vdut.rst_n = 1;
    vdut.eval();
    tfp.dump(counter++);
  }

  ~DutWrapper() { tfp.close(); }

  void ClockedBy(Clock &clk) {
    clk.AddIfDependency([this]() { always_comb(); }, {i, o});
  }

  void always_comb() {
    cout << "Dut" << endl;
    vdut.clk = 1;
    vdut.eval();

    vdut.i_valid = i->is_readable();
    if (i->is_readable()) {
      auto [i_data, i_first] = i->peek();
      if (i_first) {
        vdut.i_data = i_data;
      }
    }

    vdut.o_ready = o->is_writeable();

    vdut.eval();
    tfp.dump(counter++);

    if (vdut.i_valid and vdut.i_ready) {
      (void)i->read();
    }
    if (vdut.o_valid and vdut.o_ready) {
      o->write(vdut.o_data);
    }

    vdut.clk = 0;
    vdut.eval();
    tfp.dump(counter++);
  }
};

int main() {
  Clock clock;
  Driver driver;
  DutWrapper dut;
  Monitor monitor;
  ValidReady<unsigned, false> ch1;
  ValidReady<unsigned, false> ch2;
  driver.o = &ch1;
  dut.i = &ch1;
  dut.o = &ch2;
  monitor.i = &ch2;

  ch1.ClockedBy(clock);
  ch2.ClockedBy(clock);
  monitor.ClockedBy(clock);
  dut.ClockedBy(clock);
  driver.ClockedBy(clock);

  clock.Initialize();
  for (unsigned i = 0; i < 100; ++i) {
    cout << "cycle " << i << endl;
    clock.Comb();
    clock.FF();
  }
  return 0;
}
