
#include "VRSAMontgomery.h"
#include "assign_port.h"
#include "model_rsa.h"
#include "testbench.h"
#include "verilog/dtype.h"
#include <iostream>
#include <systemc>

using namespace std;
using namespace sc_core;

using IN = RSAMontgomeryModIn;
using DUT = VRSAMontgomery;
using OUT = RSAMontgomeryModOut;

class TestBench_RsaMontgomery : public TestBench<IN, OUT, DUT> {
public:
  using TestBench::TestBench;

  void writer(const InType &in) {
    write_verilator_port(dut_wrapper.dut->i_a, in.a);
    write_verilator_port(dut_wrapper.dut->i_b, in.b);
    write_verilator_port(dut_wrapper.dut->i_modulus, in.modulus);
  }

  OutType reader() {
    OutType out;
    read_verilator_port(out, dut_wrapper.dut->o_out);
    return out;
  }
};

int sc_main(int, char **) {
  unique_ptr<TestBench_RsaMontgomery> testbench(
      new TestBench_RsaMontgomery("testbench_montgomery_sv"));
  KeyType a;
  KeyType b;
  KeyType modulus;
  from_hex(a, "412820616369726641206874756F53202C48544542415A494C452054524F50");
  from_hex(b, "10001");
  from_hex(modulus,
           "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1");
  testbench->push_input({.a = a, .b = b, .modulus = modulus});

  TestBench_RsaMontgomery::OutType golden;
  from_hex(golden,
           "314F8ACB18E57C4B2FA37ADEFA7964711B8DCDB7AAC7514C78D97CF4D4121426");
  testbench->push_golden(golden);

  testbench->run(1200, SC_NS);
  return 0;
}