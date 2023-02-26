#include "RSA256.h"
#include "rsa.h"
#include "sc_testbench.h"
#include "verilog/dtype/vint.h"
#include <iostream>
#include <memory>
#include <systemc>

using namespace std;
using namespace sc_core;
using namespace verilog;

using IN = RSAModIn;
using OUT = RSAModOut;
using DUT = RSA256;

class ScTestbench_RSA256 : public ScTestbench<IN, OUT, DUT> {
public:
  using ScTestbench::ScTestbench;
  virtual void Connect() override {
    dut.i_data(fifo_in);
    dut.o_crypto(fifo_out);
  }
  virtual void WriteInput(const InType &in) override { fifo_in.write(in); }
  virtual OutType ReadOutput() override { return fifo_out.read(); }

private:
  sc_fifo<InType> fifo_in;
  sc_fifo<OutType> fifo_out;
};

const char str_msg[] =
    "412820616369726641206874756F53202C48544542415A494C452054524F50";
const char str_key[] = "10001";
const char str_modulus[] =
    "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1";
const char str_ans[] =
    "0D41B183313D306ADCA09126F3FED6CDEC7DCDCE49DB5C85CB2A37F08C0F2E31";

int sc_main(int, char **) {
  unique_ptr<ScTestbench_RSA256> tb =
      CreateScTestbench<ScTestbench_RSA256>("RSA256");
  RSAModIn in;
  from_hex(in.msg, str_msg);
  from_hex(in.key, str_key);
  from_hex(in.modulus, str_modulus);
  RSAModOut ans;
  from_hex(ans, str_ans);
  tb->push_golden(ans);
  tb->push_input(in);
  return tb->run(10, SC_US);
}
