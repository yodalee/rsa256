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
    rsa_key_t message = i_message.read();
    rsa_key_t key = i_key.read();
    rsa_key_t modulus = i_modulus.read();
    rsa_key_t crypto;
    rsa(crypto, message, key, modulus);
    cout << crypto << endl;
    o_crypto.write(crypto);
  }
}