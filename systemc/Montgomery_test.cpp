#include <iostream>
#include <memory>
#include <systemc>

#include "Montgomery.h"
#include "model_rsa.h"
#include "sc_testbench.h"
#include "verilog/dtype/vint.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace verilog;

using IN = MontgomeryIn;
using OUT = MontgomeryOut;
using DUT = Montgomery;

class ScTestbench_Montgomery : public ScTestbench<IN, OUT, DUT> {
public:
  using ScTestbench::ScTestbench;
  virtual void Connect() override {
    dut.data_in(fifo_in);
    dut.data_out(fifo_out);
  }
  virtual void WriteInput(const InType &in) override { fifo_in.write(in); }
  virtual OutType ReadOutput() override { return fifo_out.read(); }

private:
  sc_fifo<InType> fifo_in;
  sc_fifo<OutType> fifo_out;
};

const char str_N[] =
    "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1";
const char str_A[] =
    "412820616369726641206874756F53202C48544542415A494C452054524F50";
const char str_B[] = "10001";
const char str_ans[] =
    "314F8ACB18E57C4B2FA37ADEFA7964711B8DCDB7AAC7514C78D97CF4D4121426";

int sc_main(int, char **) {
  unique_ptr<ScTestbench_Montgomery> tb =
      CreateScTestbench<ScTestbench_Montgomery>("montgomery");
  KeyType a, b, modulus, ans;
  from_hex(a, str_A);
  from_hex(b, str_B);
  from_hex(modulus, str_N);
  from_hex(ans, str_ans);
  tb->push_input({a, b, modulus});
  tb->push_golden(ans);
  return tb->run(10, SC_US);
}
