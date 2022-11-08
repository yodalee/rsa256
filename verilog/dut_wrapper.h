#pragma once

#include <functional>
#include <systemc>
#include <vector>

#include "verilated_vcd_c.h"

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
  DUTWrapper(const sc_module_name &name, DUT *dut_)
      : sc_module(name), dut(dut_) {
    SC_THREAD(Executor);
  }

  sc_in_clk clk;
  unique_ptr<DUT> dut;

  void Executor() {
    for (auto callback : this->callback_init) {
      callback(this->dut.get());
    }

    while (true) {
      wait(this->clk.posedge_event());
      for (auto callback : this->callback_beforeclk) {
        callback(this->dut.get());
      }
      dut->clk = true;
      dut->eval();
      for (auto callback : this->callback_afterclk) {
        callback(this->dut.get());
      }
      wait(1.0, SC_NS);
      dut->clk = false;
      dut->eval();
      wait(1.0, SC_NS);
    }
  }
};
