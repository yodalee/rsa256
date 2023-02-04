
#include "VRSA.h"
#include "assign_port.h"
#include "model_rsa.h"
#include "testbench.h"
#include "verilog/dtype.h"
#include <iostream>
#include <systemc>

using namespace std;
using namespace sc_core;

using IN = RSAModIn;
using DUT = VRSA;
using OUT = RSAModOut;

class TestBench_Rsa : public TestBench<IN, OUT, DUT> {
public:
  using TestBench::TestBench;

  void writer(const InType &in) {
    write_verilator_port(dut_wrapper.dut->i_msg, in.msg);
    write_verilator_port(dut_wrapper.dut->i_key, in.key);
    write_verilator_port(dut_wrapper.dut->i_modulus, in.modulus);
  }

  OutType reader() {
    OutType out;
    read_verilator_port(out, dut_wrapper.dut->o_crypto);
    return out;
  }
};

int sc_main(int, char **) {
  unique_ptr<TestBench_Rsa> testbench(new TestBench_Rsa("testbench_rsa_sv"));
  testbench->run(1200, SC_NS);
  return 0;
}