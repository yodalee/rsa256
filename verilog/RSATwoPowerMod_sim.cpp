
#include "VRSATwoPowerMod.h"
#include "bridge/verilator/verilator_assign.h"
#include "model_rsa.h"
#include "bridge/verilator/testbench.h"
#include "verilog/dtype.h"
#include <iostream>
#include <systemc>

using namespace std;
using namespace sc_core;
using namespace verilog::verilator;

using IN = RSATwoPowerModIn;
using DUT = VRSATwoPowerMod;
using OUT = RSATwoPowerModOut;

class TestBench_RsaTwoPowerMod : public TestBench<IN, OUT, DUT> {
public:
  using TestBench::TestBench;

  virtual void writer(const InType &in) {
    write_port(dut_wrapper.dut->i_in, verilog::pack(in));
  };

  virtual OutType reader() {
    OutType out;
    read_port(out, dut_wrapper.dut->o_out);
    return out;
  };
};

int sc_main(int, char **) {
  unique_ptr<TestBench_RsaTwoPowerMod> testbench(new TestBench_RsaTwoPowerMod(
      "TestBench_two_power_mod_sv", /*dump=*/true));
  KeyType modulus;
  from_hex(modulus,
           "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1");
  testbench->push_input(
      {.power = RSATwoPowerModIn::IntType(512), .modulus = modulus});

  TestBench_RsaTwoPowerMod::OutType golden;
  from_hex(golden,
           "0AF39E1F831CB4FCD92B17F61F473735C687593A931C97D2B60AD6C7443F09FDB");
  testbench->push_golden(golden);

  return testbench->run(1200, SC_NS);
}
