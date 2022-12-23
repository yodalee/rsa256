#pragma once

#include <systemc>

#include "model_rsa.h"
#include "verilog_int.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace verilog;

SC_MODULE(RSATwoPowerMod) {
  sc_in_clk clk;
  sc_fifo_in<RSATwoPowerModIn> data_in;
  sc_fifo_out<RSATwoPowerModOut> data_out;

  SC_CTOR(RSATwoPowerMod) { SC_THREAD(Thread); }

  void Thread();
};