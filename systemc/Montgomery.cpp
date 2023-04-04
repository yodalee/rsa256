#include <systemc>

#include "Montgomery.h"
#include "verilog/dtype.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace verilog;

void Montgomery::Thread() {
  while (true) {
    MontgomeryIn in = data_in.read();
    KeyType a = in.a;
    ExtendKeyType b = static_cast<ExtendKeyType>(in.b);
    ExtendKeyType modulus = static_cast<ExtendKeyType>(in.modulus);
    ExtendKeyType round_result(0);
    for (int i = 0; i < kBW; ++i) {
      if (a.Bit(i)) {
        round_result += b;
      }
      if (round_result.Bit(0)) {
        round_result += modulus;
      }
      round_result >>= 1;
    }
    if (round_result > modulus) {
      round_result -= modulus;
    }
    data_out.write(static_cast<KeyType>(round_result));
  }
}
