#pragma once

#include <functional>
#include <systemc>
#include <vector>
#if VM_TRACE
#include <verilated_fst_c.h>
#endif

using namespace std;
using namespace sc_core;

enum CallbackPhase {
  Init,
  BeforeClk,
  AfterClk,
};

template <typename DUT> SC_MODULE(DUTWrapper) {
public:
  typedef function<void(DUT *)> Callback;
  vector<Callback> callback_init;
  vector<Callback> callback_beforeclk;
  vector<Callback> callback_afterclk;

  void register_callback(CallbackPhase phase, Callback callback) {
    switch (phase) {
    case CallbackPhase::Init:
      this->callback_init.push_back(callback);
      break;
    case CallbackPhase::BeforeClk:
      this->callback_beforeclk.push_back(callback);
      break;
    case CallbackPhase::AfterClk:
      this->callback_afterclk.push_back(callback);
      break;
    default:
      assert(false);
    }
  }

  SC_HAS_PROCESS(DUTWrapper);
  DUTWrapper(const sc_module_name &name)
      : sc_module(name), ctx(new VerilatedContext),
       dut(new DUT(ctx.get())), tfp(new VerilatedFstC) {
    Verilated::traceEverOn(true);
    ctx->traceEverOn(true);
    dut->trace(tfp.get(), 99);  // Trace 99 levels of hierarchy (or see below)
    std::string filename = std::string((const char *)name) + "_dump.fst";
    tfp->open(filename.c_str());
    SC_THREAD(Executor);
  }

 	~DUTWrapper() {
		tfp->close();
	}

  sc_in_clk clk;
  unique_ptr<VerilatedContext> ctx;
  unique_ptr<DUT> dut;
  unique_ptr<VerilatedFstC> tfp;

  void Executor() {
    for (auto callback : this->callback_init) {
      callback(this->dut.get());
    }

    while (true) {
      wait(this->clk.posedge_event());
      for (auto callback : this->callback_beforeclk) {
        callback(this->dut.get());
      }
      ctx->timeInc(1);
      dut->clk = true;
      tfp->dump(ctx->time());
      dut->eval();
      for (auto callback : this->callback_afterclk) {
        callback(this->dut.get());
      }
      wait(1.0, SC_NS);
      ctx->timeInc(1);
      dut->clk = false;
      tfp->dump(ctx->time());
      dut->eval();
      wait(1.0, SC_NS);
    }
  }
};
