#pragma once

#include "connector.h"
#include <functional>
#include <glog/logging.h>
#include <memory>
#include <vector>
#include <verilated_fst_c.h>

using namespace std;

template <typename DUT> class DUTWrapper {
public:
  const unsigned period_ps = 2;
  vector<shared_ptr<Connector<DUT>>> connectors;
  bool dump_waveform;

  void register_connector(shared_ptr<Connector<DUT>> connector) {
    this->connectors.push_back(connector);
  }

  DUTWrapper(const std::string &name, const bool dump = false)
      : ctx(new VerilatedContext), dut(new DUT(ctx.get())),
        tfp(new VerilatedFstC) {
    dump_waveform = dump;
    if (dump_waveform) {
      // initialize waveform
      Verilated::traceEverOn(true);
      ctx->traceEverOn(true);
      dut->trace(tfp.get(), 99); // Trace 99 levels of hierarchy (or see below)
      std::string filename = name + "_dump.fst";
      tfp->open(filename.c_str());
    }
  }

  ~DUTWrapper() {
    tfp->flush();
    tfp->close();
  }

  bool clk;
  unique_ptr<VerilatedContext> ctx;
  unique_ptr<DUT> dut;
  unique_ptr<VerilatedFstC> tfp;

  void Step() {
    dut->eval();
    if (dump_waveform) {
      tfp->dump(ctx->time());
    }
  }

  void Init() {
    // simulate the initialize value
    dut->clk = 0;
    dut->rst_n = 1;
    Step();

    // pull down the reset value
    ctx->timeInc(period_ps);
    dut->rst_n = 0;
    Step();

    ctx->timeInc(period_ps);
    dut->rst_n = 1;
    Step();

    ctx->timeInc(period_ps);
  }

  // TODO: add heartbeat module and change num_steps to time
  void Run(size_t num_steps) {
    for (size_t _ = 0; _ < num_steps; _++) {
      LOG(INFO) << "Step: @" << ctx->time();
      for (auto &connector : this->connectors) {
        connector->before_clk(dut.get());
      }
      dut->clk = true;
      dut->eval();

      bool update = false;
      for (auto &connector : this->connectors) {
        update |= connector->after_clk(dut.get());
      }
      if (update) {
        dut->eval();
      }
      if (dump_waveform) {
        tfp->dump(ctx->time());
      }
      ctx->timeInc(period_ps);

      // negtive edge of clk
      dut->clk = false;
      Step();
      ctx->timeInc(period_ps);
    }
  }

  bool is_pass() {
    bool is_pass = true;
    for (const auto &connector : this->connectors) {
      is_pass &= connector->is_pass(dut.get());
    }
    return is_pass;
  }
};
