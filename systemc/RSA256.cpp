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
    RSAModIn in = i_data.read();
    vuint<kBW + 2> out;
    rsa(out, static_cast<vuint<kBW + 2>>(in.msg),
        static_cast<vuint<kBW + 2>>(in.key),
        static_cast<vuint<kBW + 2>>(in.modulus));
    KeyType crypto = static_cast<vuint<kBW>>(out);
    cout << crypto << endl;
    o_crypto.write(crypto);
  }
}