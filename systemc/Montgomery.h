#pragma once

#include <systemc>

#include "model_rsa.h"
#include "verilog/dtype/vint.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace verilog;

SC_MODULE(Montgomery) {
  using ExtendKeyType = vuint<kBW + 2>;

  sc_in_clk clk;
  sc_fifo_in<MontgomeryIn> data_in;
  sc_fifo_out<MontgomeryOut> data_out;

  SC_CTOR(Montgomery) { SC_THREAD(Thread); }

  void Thread();
};
