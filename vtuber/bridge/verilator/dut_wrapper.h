#pragma once

#include "connector.h"
#include <functional>
#include <memory>
#include <systemc>
#include <vector>
#if VM_TRACE
#include <verilated_fst_c.h>
#endif

using namespace std;
using namespace sc_core;

template <typename DUT> SC_MODULE(DUTWrapper) {
public:
  vector<shared_ptr<Connector>> connectors;
  bool dump_waveform;

  void register_connector(shared_ptr<Connector> connector) {
    this->connectors.push_back(connector);
  }

  SC_HAS_PROCESS(DUTWrapper);
  DUTWrapper(const sc_module_name &name, const bool dump = false)
      : sc_module(name), ctx(new VerilatedContext), dut(new DUT(ctx.get())),
        tfp(new VerilatedFstC) {
    dump_waveform = dump;
    Init();
    SC_THREAD(Executor);
  }

  ~DUTWrapper() {
    tfp->flush();
    tfp->close();
  }

  sc_in_clk clk;
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
    if (dump_waveform) {
      // initialize waveform
      Verilated::traceEverOn(true);
      ctx->traceEverOn(true);
      dut->trace(tfp.get(), 99); // Trace 99 levels of hierarchy (or see below)
      std::string filename = std::string(dut->modelName()) + "_dump.fst";
      tfp->open(filename.c_str());
    }

    // simulate the initialize value
    dut->clk = 0;
    dut->rst = 1;
    Step();

    // pull down the reset value
    ctx->timeInc(1);
    dut->rst = 0;
    Step();

    ctx->timeInc(1);
    dut->rst = 1;
    Step();

    ctx->timeInc(1);
  }

  void Executor() {
    while (true) {
      wait(this->clk.posedge_event());

      for (auto &connector : this->connectors) {
        connector->before_clk();
      }
      dut->clk = true;
      dut->eval();

      bool update = false;
      for (auto &connector : this->connectors) {
        update |= connector->after_clk();
      }
      if (update) {
        dut->eval();
      }
      if (dump_waveform) {
        tfp->dump(ctx->time());
      }
      ctx->timeInc(1);

      // negtive edge of clk
      dut->clk = false;
      Step();
      ctx->timeInc(1);
    }
  }
};
