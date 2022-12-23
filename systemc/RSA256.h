#include "model_rsa.h"
#include "verilog_int.h"
#include <systemc>

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace verilog;

SC_MODULE(RSA256) {
  sc_in_clk clk;
  sc_fifo_in<RSAModIn> i_data;
  sc_fifo_out<KeyType> o_crypto;

  SC_CTOR(RSA256) { SC_THREAD(Thread); }

  void Thread();
};