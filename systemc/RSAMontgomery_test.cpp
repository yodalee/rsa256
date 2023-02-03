#include <iostream>
#include <memory>
#include <systemc>

#include "RSAMontgomery.h"
#include "model_rsa.h"
#include "verilog/dtype/vint.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace verilog;

const char str_N[] =
    "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1";
const char str_A[] =
    "412820616369726641206874756F53202C48544542415A494C452054524F50";
const char str_B[] = "10001";
const char str_ans[] =
    "314F8ACB18E57C4B2FA37ADEFA7964711B8DCDB7AAC7514C78D97CF4D4121426";

SC_MODULE(Testbench) {
  sc_clock clk;
  sc_fifo<RSAMontgomeryModIn> data_in;
  sc_fifo<RSAMontgomeryModOut> data_out;
  RSAMontgomery montgomery_mod;
  bool timeout, pass;

  SC_CTOR(Testbench)
      : clk("clk", 1, SC_NS), montgomery_mod("montgomery"), timeout(true),
        pass(true) {
    SC_THREAD(Driver);
    SC_THREAD(Monitor);
    montgomery_mod.clk(clk);
    montgomery_mod.data_in(data_in);
    montgomery_mod.data_out(data_out);
  }

  void Driver() {
    KeyType a, b, modulus;
    cout << "calculate montgomery: " << endl;
    cout << str_A << " X " << str_B << " mod " << str_N << endl;
    from_hex(a, str_A);
    from_hex(b, str_B);
    from_hex(modulus, str_N);
    data_in.write({a, b, modulus});
  }

  void Monitor() {
    KeyType out = data_out.read();
    string str_out = to_hex(out);
    cout << "Output: " << str_out << endl;
    if (str_out == str_ans) {
      cout << "OK" << endl;
    } else {
      cout << "Golden != systemC: " << str_out << " vs " << str_ans << endl;
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
