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
    vuint<kBW> crypto;
    rsa(crypto, in.msg, in.key, in.modulus);
    cout << crypto << endl;
    o_crypto.write(crypto);
  }
}