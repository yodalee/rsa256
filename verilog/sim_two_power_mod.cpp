
#include "Vtwo_power_mod.h"
#include "assign_port.h"
#include "model_rsa.h"
#include "test_fixture.h"
#include "verilog_int.h"
#include <iostream>
#include <systemc>

using namespace std;
using namespace sc_core;

using IN = RSATwoPowerModIn;
using DUT = Vtwo_power_mod;
using OUT = RSATwoPowerModOut;

class TestBench_RsaTwoPowerMod : public TestBench<IN, OUT, DUT> {
public:
  using TestBench::TestBench;

  virtual void writer(const InType &in) {
    write_verilator_port(dut_wrapper.dut->i_power, in.power);
    write_verilator_port(dut_wrapper.dut->i_modulus, in.modulus);
  };

  virtual OutType reader() {
    OutType out;
    read_verilator_port(out, dut_wrapper.dut->o_out);
    return out;
  };
};

int sc_main(int, char **) {
  unique_ptr<TestBench_RsaTwoPowerMod> testbench(
      new TestBench_RsaTwoPowerMod("TestBench_two_power_mod_sv"));
  KeyType modulus;
  from_hex(modulus,
           "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1");
  testbench->push_input({.power = verilog::vuint<32>(512), .modulus = modulus});

  TestBench_RsaTwoPowerMod::OutType golden;
  from_hex(golden,
           "0AF39E1F831CB4FCD92B17F61F473735C687593A931C97D2B60AD6C7443F09FDB");
  testbench->push_golden(golden);

  sc_start(1200.0, SC_NS);
  return 0;
}