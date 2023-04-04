#include "RSAMont.h"
#include "TwoPower.h"
#include "model_rsa.h"
#include "verilog/dtype/vint.h"
#include <systemc>

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace verilog;

SC_MODULE(RSA) {
  sc_in_clk clk;
  sc_fifo_in<RSAModIn> i_data;
  sc_fifo_out<KeyType> o_crypto;

  TwoPower i_two_power_mod;
  RSAMont i_mont;
  sc_fifo<TwoPowerIn> two_power_mod_in;
  sc_fifo<TwoPowerOut> two_power_mod_out;
  sc_fifo<RSAMontModIn> mont_in;
  sc_fifo<RSAMontModOut> mont_out;

  SC_CTOR(RSA) : i_two_power_mod("i_two_power_mod"), i_mont("i_mont") {
    i_two_power_mod.clk(clk);
    i_two_power_mod.data_in(two_power_mod_in);
    i_two_power_mod.data_out(two_power_mod_out);
    i_mont.clk(clk);
    i_mont.data_in(mont_in);
    i_mont.data_out(mont_out);
    SC_THREAD(Thread);
  }

  void Thread();
};
