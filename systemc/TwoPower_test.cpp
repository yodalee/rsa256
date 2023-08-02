#include <iostream>
#include <memory>
#include <systemc>

#include "TwoPower.h"
#include "model_rsa.h"
#include "sc_testbench.h"
#include "verilog/dtype/vint.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace verilog;

using IN = TwoPowerIn;
using OUT = TwoPowerOut;
using DUT = TwoPower;

class ScTestbench_TwoPower : public ScTestbench<IN, OUT, DUT> {
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

const char str_modulus[] =
    "0xE07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1";
const char str_ans[] =
    "AF39E1F831CB4FCD92B17F61F473735C687593A931C97D2B60AD6C7443F09FDB";

int sc_main(int, char **) {
  unique_ptr<ScTestbench_TwoPower> tb =
      CreateScTestbench<ScTestbench_TwoPower>("TwoPower");
  IntType power{512};
  KeyType modulus;
  KeyType ans;
  from_hex(modulus, str_modulus);
  from_hex(ans, str_ans);
  tb->push_input({power, modulus});
  tb->push_golden(ans);
  return tb->run(10, SC_US);
}
