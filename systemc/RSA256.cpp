#include "RSA256.h"
#include "rsa.h"
#include <systemc>

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace verilog;

void RSA256::Thread() {
  while (true) {
    const RSAModIn &in = i_data.read();
    const RSATwoPowerModIn::IntType vuint512{512};
    two_power_mod_in.write({.power = vuint512, .modulus = in.modulus});

    auto pack_value = two_power_mod_out.read();
    mont_in.write({.base = pack_value,
                   .msg = in.msg,
                   .key = in.key,
                   .modulus = in.modulus});
    o_crypto.write(mont_out.read());
  }
}