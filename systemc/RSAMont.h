#pragma once

#include <systemc>

#include "RSAMontgomery.h"
#include "model_rsa.h"
#include "verilog/dtype/vint.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace verilog;

SC_MODULE(RSAMont) {
  sc_in_clk clk;
  sc_fifo_in<RSAMontModIn> data_in;
  sc_fifo_out<RSAMontModOut> data_out;

  RSAMontgomery i_montgomery;
  sc_fifo<RSAMontgomeryModIn> montgomery_in;
  sc_fifo<RSAMontgomeryModOut> montgomery_out;

  SC_CTOR(RSAMont) : i_montgomery("i_montgomery") {
    i_montgomery.clk(clk);
    i_montgomery.data_in(montgomery_in);
    i_montgomery.data_out(montgomery_out);
    SC_THREAD(Thread);
  }

  void Thread();
};
