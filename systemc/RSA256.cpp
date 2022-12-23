#include "RSA256.h"
#include "rsa.h"
#include <systemc>

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace verilog;

void RSA256::Thread() {
  char str[256];
  while (true) {
    const RSAModIn &in = i_data.read();
    const RSATwoPowerModIn::TwoPowerMod_Power_t vuint512{512};
    two_power_mod_in.write({.power = vuint512, .modulus = in.modulus});

    auto pack_value = two_power_mod_out.read();
    montgomery_in.write({.a = in.msg, .b = pack_value, .modulus = in.modulus});
    auto packed_msg = montgomery_out.read();
    const auto &key = in.key;

    KeyType crypto;
    KeyType multiple{1};
    KeyType square{packed_msg};
    for (size_t i = 0; i < kBW; i++) {
      if (key.Bit(i)) {
        montgomery_in.write(
            {.a = multiple, .b = square, .modulus = in.modulus});
        multiple = montgomery_out.read();
      }
      montgomery_in.write({.a = square, .b = square, .modulus = in.modulus});
      square = montgomery_out.read();
    }
    crypto = multiple;
    o_crypto.write(crypto);
  }
}