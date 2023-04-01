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
      if (i == 0) {
        // 2^2n * msg mod N
        montgomery_in.write({.a = msg, .b = base, .modulus = modulus});
        square = montgomery_out.read();
      } else if ((i & 1)) {
        // multiply
        montgomery_in.write({.a = multiple, .b = square, .modulus = modulus});
        const int key_idx = (i - 1) / 2;
        if (key.Bit(key_idx)) {
          multiple = montgomery_out.read();
        } else {
          montgomery_out.read();
        }
      } else {
        // square
        montgomery_in.write({.a = square, .b = square, .modulus = modulus});
        square = montgomery_out.read();
      }
    }
    data_out.write(static_cast<KeyType>(multiple));
  }
}
