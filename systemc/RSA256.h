#include "RSAMontgomery.h"
#include "RSATwoPowerMod.h"
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

  RSAMontgomery i_montgomery;
  RSATwoPowerMod i_two_power_mod;
  sc_fifo<RSATwoPowerModIn> two_power_mod_in;
  sc_fifo<RSATwoPowerModOut> two_power_mod_out;
  sc_fifo<RSAMontgomeryModIn> montgomery_in;
  sc_fifo<RSAMontgomeryModOut> montgomery_out;

  SC_CTOR(RSA256)
      : i_montgomery("i_montgomery"), i_two_power_mod("i_two_power_mod") {
    i_two_power_mod.clk(clk);
    i_two_power_mod.data_in(two_power_mod_in);
    i_two_power_mod.data_out(two_power_mod_out);
    i_montgomery.clk(clk);
    i_montgomery.data_in(montgomery_in);
    i_montgomery.data_out(montgomery_out);
    SC_THREAD(Thread);
  }

  void Thread();
};