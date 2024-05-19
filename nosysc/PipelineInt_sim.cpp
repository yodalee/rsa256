#include "VPipelineInt.h"

#include "nosysc.h"
#include "verilated_fst_c.h"
#include "vrdriver.h"
#include "vrmonitor.h"

#include <glog/logging.h>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <numeric>
#include <queue>
#include <type_traits>
#include <utility>

using namespace std;
using namespace nosysc;

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
  VPipelineInt vdut;
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
    DLOG(INFO) << "Dut Comb";
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

TEST(NosyscTest, PipelineTest) {
  vector<unsigned> outs;
  Clock clock;
  VRDriver driver;
  Dut dut;
  VRMonitor monitor([&](unsigned d) { outs.push_back(d); });
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
    DLOG(INFO) << "Cycle: " << i;
    clock.Comb();
    clock.FF();
  }

  vector<unsigned> goldens(kLIMIT);
  iota(goldens.begin(), goldens.end(), 0);
  EXPECT_EQ(outs, goldens);
}

TEST(NosyscTest, PipelineVerilogTest) {
  vector<unsigned> outs;
  Clock clock;
  VRDriver driver;
  DutWrapper dut;
  VRMonitor monitor([&](unsigned d) { outs.push_back(d); });
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
    DLOG(INFO) << "Cycle: " << i;
    clock.Comb();
    clock.FF();
  }

  vector<unsigned> goldens(kLIMIT);
  iota(goldens.begin(), goldens.end(), 0);
  EXPECT_EQ(outs, goldens);
}

TEST(NosyscTest, RandPipelineTest) {
  vector<unsigned> outs;
  Clock clock;
  VRDriver driver;
  DutWrapper dut;
  VRMonitor monitor([&](unsigned d) { outs.push_back(d); });
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
  driver.SetRandomValidPolicy(random_factory::OneEvery(5));
  monitor.SetRandomValidPolicy(random_factory::OneEvery(10));

  clock.Initialize();
  for (unsigned i = 0; i < 150; ++i) {
    DLOG(INFO) << "Cycle: " << i;
    clock.Comb();
    clock.FF();
  }

  vector<unsigned> goldens(kLIMIT);
  iota(goldens.begin(), goldens.end(), 0);
  EXPECT_EQ(outs, goldens);
}
