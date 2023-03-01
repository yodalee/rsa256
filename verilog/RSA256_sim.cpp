
#include "VRSA.h"
#include "verilator_assign.h"
#include "model_rsa.h"
#include "testbench.h"
#include "verilog/dtype.h"
#include <iostream>
#include <systemc>

using namespace std;
using namespace sc_core;
using namespace verilog::verilator;

using IN = RSAModIn;
using DUT = VRSA;
using OUT = RSAModOut;

class TestBench_Rsa : public TestBench<IN, OUT, DUT> {
public:
  using TestBench::TestBench;

  void writer(const InType &in) {
    write_port(dut_wrapper.dut->i_in, verilog::pack(in));
  }

  OutType reader() {
    OutType out;
    read_port(out, dut_wrapper.dut->o_out);
    return out;
  }
};

const char str_msg[] =
    "412820616369726641206874756F53202C48544542415A494C452054524F50";
const char str_key[] = "10001";
const char str_modulus[] =
    "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1";
const char str_ans[] =
    "0D41B183313D306ADCA09126F3FED6CDEC7DCDCE49DB5C85CB2A37F08C0F2E31";

int sc_main(int, char **) {
  unique_ptr<TestBench_Rsa> testbench(
      new TestBench_Rsa("testbench_rsa_sv", /*dump=*/true));
  // sample in
  RSAModIn in;
  from_hex(in.msg, str_msg);
  from_hex(in.key, str_key);
  from_hex(in.modulus, str_modulus);
  testbench->push_input(in);
  // sample out
  RSAModOut ans;
  from_hex(ans, str_ans);
  testbench->push_golden(ans);
  return testbench->run(180 * 1000, SC_NS);
}