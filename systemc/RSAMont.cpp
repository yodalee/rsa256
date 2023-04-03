#include <systemc>

#include "RSAMont.h"
#include "verilog/dtype.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace verilog;

void RSAMont::Thread() {
  while (true) {
    RSAMontModIn in = data_in.read();
    KeyType base = in.base;
    KeyType msg = in.msg;
    KeyType key = in.key;
    KeyType modulus = in.modulus;
    KeyType multiple{1};
    KeyType square{0};

    for (int i = 0; i < kBW * 2 + 1; i++) {
      KeyType mont_in[2];
      if (i == 0) {
        // 2^2n * msg mod N
        mont_in[0] = msg;
        mont_in[1] = base;
      } else if ((i & 1)) {
        // multiply
        mont_in[0] = multiple;
        mont_in[1] = square;
      } else {
        // square
        mont_in[0] = square;
        mont_in[1] = square;
      }
      montgomery_in.write(
          {.a = mont_in[0], .b = mont_in[1], .modulus = modulus});
      KeyType result = montgomery_out.read();
      if (i == 0) {
        square = result;
      } else if ((i & 1)) {
        const int key_idx = (i - 1) / 2;
        if (key.Bit(key_idx)) {
          multiple = result;
        }
      } else {
        square = result;
      }
    }
    data_out.write(static_cast<KeyType>(multiple));
  }
}
