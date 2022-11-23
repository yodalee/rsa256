#include <iostream>
#include <memory>
#include <systemc>

#include "rsa.h"
#include "verilog_int.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace verilog;

constexpr int kBW = 256;
using KeyType = vint<false, 257>;
struct RSATwoPowerModIn {
  friend ::std::ostream &operator<<(::std::ostream &os,
                                    const RSATwoPowerModIn &v) {
    os << "{" << v.power << ", " << v.modulus << "}" << std::endl;
    return os;
  }

  sc_uint<32> power;
  KeyType modulus;
};
using RSATwoPowerModOut = KeyType;

typedef vint<false, 257> KeyType;

SC_MODULE(RSATwoPowerMod) {
  sc_in_clk clk;
  sc_fifo_in<RSATwoPowerModIn> data_in;
  sc_fifo_out<RSATwoPowerModOut> data_out;

  SC_CTOR(RSATwoPowerMod) { SC_THREAD(Thread); }

  void Thread() {
    KeyType round_result;
    while (true) {
      RSATwoPowerModIn in = data_in.read();
      int power = in.power;
      KeyType modulus = in.modulus;

      from_hex(round_result, "1");
      for (size_t i = 0; i < power; i++) {
        round_result <<= 1;
        if (round_result > modulus) {
          round_result -= modulus;
        }
      }
      data_out.write(round_result);
    }
  }
};

const char str_modulus[] =
    "0xE07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1";
const char str_ans[] =
    "AF39E1F831CB4FCD92B17F61F473735C687593A931C97D2B60AD6C7443F09FDB";
const int power = 512;
SC_MODULE(Testbench) {

  sc_clock clk;
  sc_fifo<RSATwoPowerModIn> data_in;
  sc_fifo<RSATwoPowerModOut> data_out;
  RSATwoPowerMod two_power_mod;
  bool timeout, pass;

  SC_CTOR(Testbench)
      : clk("clk", 1, SC_NS), two_power_mod("two_power_mod"), timeout(true),
        pass(true) {
    SC_THREAD(Driver);
    SC_THREAD(Monitor);
    two_power_mod.clk(clk);
    two_power_mod.data_in(data_in);
    two_power_mod.data_out(data_out);
  }

  void Driver() {
    sc_uint<32> power = 512;
    KeyType modulus;
    cout << "calculate 2^: " << power << endl;
    cout << "modulus: " << str_modulus << endl;
    from_hex(modulus, str_modulus);
    data_in.write({power, modulus});
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
