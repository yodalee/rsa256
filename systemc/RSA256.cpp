#include "rsa.h"
#include "verilog_int.h"
#include <iostream>
#include <memory>
#include <systemc>

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace verilog;

SC_MODULE(RSA256) {
  sc_in_clk clk;
  sc_fifo_in<rsa_key_t> i_message;
  sc_fifo_in<rsa_key_t> i_key;
  sc_fifo_in<rsa_key_t> i_modulus;
  sc_fifo_out<rsa_key_t> o_crypto;

  SC_CTOR(RSA256) { SC_THREAD(Thread); }

  void Thread() {
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
};

const char str_msg[] =
    "412820616369726641206874756F53202C48544542415A494C452054524F50";
const char str_key[] = "10001";
const char str_modulus[] =
    "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1";
const char str_ans[] =
    "0D41B183313D306ADCA09126F3FED6CDEC7DCDCE49DB5C85CB2A37F08C0F2E31";
SC_MODULE(Testbench) {
  sc_clock clk;
  sc_fifo<rsa_key_t> i_message;
  sc_fifo<rsa_key_t> i_key;
  sc_fifo<rsa_key_t> i_modulus;
  sc_fifo<rsa_key_t> o_crypto;
  RSA256 rsa;
  bool timeout, pass;

  SC_CTOR(Testbench)
      : clk("clk", 1, SC_NS), rsa("rsa"), timeout(true), pass(true) {
    SC_THREAD(Driver);
    SC_THREAD(Monitor);
    rsa.clk(clk);
    rsa.i_message(i_message);
    rsa.i_key(i_key);
    rsa.i_modulus(i_modulus);
    rsa.o_crypto(o_crypto);
  }

  void Driver() {
    cout << "Message: " << str_msg << endl;
    cout << "Key: " << str_key << endl;
    cout << "modulus: " << str_modulus << endl;
    rsa_key_t message, key, modulus;
    from_hex(message, str_msg);
    from_hex(key, str_key);
    from_hex(modulus, str_modulus);
    i_message.write(message);
    i_key.write(key);
    i_modulus.write(modulus);
  }

  void Monitor() {
    rsa_key_t gotten = o_crypto.read();
    rsa_key_t ans;
    from_hex(ans, str_ans);
    if (gotten == ans) {
      cout << "OK" << endl;
    } else {
      cout << "Golden != systemC: " << ans << " vs " << gotten << endl;
      pass = false;
    }
    timeout = false;
    sc_stop();
  }
};

int sc_main(int, char **) {
  unique_ptr<Testbench> tb(new Testbench("testbench"));
  sc_start(10, SC_US);
  return 0;
}
