#include "nosysc.h"
#include "vrdriver.h"
#include "vrmonitor.h"

#include <glog/logging.h>
#include <gtest/gtest.h>

using namespace std;
using namespace nosysc;

struct Add1 {
  ValidReadyInIf<unsigned> *i;
  ValidReadyOutIf<unsigned> *o;

  void ClockedBy(Clock &clk) {
    clk.AddIfDependency([this]() { always_comb(); }, {i, o});
  }

  void always_comb() {
    if (o->is_writeable() and i->is_readable()) {
      o->write(i->read() + 1);
    }
  }
};

struct Repeat {
  ValidReadyInIf<unsigned> *i;
  ValidReadyOutIf<unsigned> *o;
  unsigned repeat_value;
  unsigned counter;

  void ClockedBy(Clock &clk) {
    clk.AddIfDependency([this]() { always_comb(); }, {i, o});
  }

  void always_comb() {
    if (i->is_readable() and counter == 0) {
      repeat_value = i->read();
    }
    if (o->is_writeable() and counter != 0) {
      o->write(repeat_value);
    }
  }
};

struct Dut {
  Add1 add;
  Repeat repeat;

  ValidReadyInIf<unsigned> *i;
  ValidReadyOutIf<unsigned> *o;
  ValidReady<unsigned, false> ch;

  void ClockedBy(Clock &clk) {
    add.ClockedBy(clk);
    repeat.ClockedBy(clk);
  }

  void always_comb() {}
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

  vector<unsigned> goldens;
  for (int i = 0; i < kLIMIT; i++) {
    for (int j = 0; j < i + 1; j++) {
      goldens.push_back(j);
    }
  }
  EXPECT_EQ(outs, goldens);
}
