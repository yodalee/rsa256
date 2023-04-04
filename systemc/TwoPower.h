#pragma once

#include <systemc>

#include "model_rsa.h"
#include "verilog/dtype/vint.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace verilog;

SC_MODULE(TwoPower) {
  sc_in_clk clk;
  sc_fifo_in<TwoPowerIn> data_in;
  sc_fifo_out<TwoPowerOut> data_out;

  SC_CTOR(TwoPower) { SC_THREAD(Thread); }

  void Thread();
};
