
#include "Vtwo_power_mod.h"
#include "callback.h"
#include "dut_wrapper.h"
#include "verilog_int.h"
#include <iostream>
#include <systemc>

using namespace std;
using namespace sc_core;

using DUT = Vtwo_power_mod;
using KeyType = verilog::vuint<256>;
struct RSATwoPowerModIn {
  friend ::std::ostream &operator<<(::std::ostream &os,
                                    const RSATwoPowerModIn &v) {
    os << "{" << v.power << ", " << v.modulus << "}" << std::endl;
    return os;
  }

  verilog::vuint<32> power;
  KeyType modulus;
};
using RSATwoPowerModOut = KeyType;

const char str_modulus[] =
    "0xE07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1";
const char str_ans[] =
    "0AF39E1F831CB4FCD92B17F61F473735C687593A931C97D2B60AD6C7443F09FDB";
const int power = 512;

SC_MODULE(Testbench) {
  sc_clock clk;
  DUTWrapper<DUT> dut_wrapper;

public:
  SC_HAS_PROCESS(Testbench);
  Testbench(const sc_module_name &name)
      : sc_module(name), clk("clk", 1.0, SC_NS), dut_wrapper("dut_wrapper") {
    dut_wrapper.clk(clk);
    Driver<RSATwoPowerModIn> driver(
        dut_wrapper.dut->i_valid, dut_wrapper.dut->i_ready,
        [this](const RSATwoPowerModIn &in) { this->writer(in); });
    Monitor<RSATwoPowerModOut> monitor(dut_wrapper.dut->o_valid,
                                       dut_wrapper.dut->o_ready,
                                       [this]() { return this->reader(); });
    dut_wrapper.register_callback(driver);
    dut_wrapper.register_callback(monitor);
  }
  void writer(const RSATwoPowerModIn &in) {}

  RSATwoPowerModOut reader() {}
};

int sc_main(int, char **) {
  unique_ptr<Testbench> testbench(new Testbench("testbench"));
  sc_start(10.0, SC_NS);
  return 0;
}
