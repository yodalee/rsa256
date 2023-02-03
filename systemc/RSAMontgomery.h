#pragma once

#include <systemc>

#include "model_rsa.h"
#include "verilog/dtype/vint.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace verilog;

SC_MODULE(RSAMontgomery) {
  using ExtendKeyType = vuint<kBW + 2>;

  sc_in_clk clk;
  sc_fifo_in<RSAMontgomeryModIn> data_in;
  sc_fifo_out<RSAMontgomeryModOut> data_out;

  SC_CTOR(RSAMontgomery) { SC_THREAD(Thread); }

  void Thread();
};
