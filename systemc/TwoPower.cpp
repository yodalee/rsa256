#include <systemc>

#include "TwoPower.h"
#include "model_rsa.h"
#include "verilog/dtype/vint.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace verilog;

void TwoPower::Thread() {
  using ExtendKeyType = vuint<kBW + 1>;
  ExtendKeyType round_result;
  while (true) {
    TwoPowerIn in = data_in.read();
    int power = in.power.value();
    ExtendKeyType modulus = static_cast<ExtendKeyType>(in.modulus);

    from_hex(round_result, "1");
    for (size_t i = 0; i < power; i++) {
      round_result <<= 1;
      if (round_result > modulus) {
        round_result -= modulus;
      }
    }
    data_out.write(static_cast<KeyType>(round_result));
  }
}
