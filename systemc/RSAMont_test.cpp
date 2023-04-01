#include <iostream>
#include <memory>
#include <systemc>

#include "RSAMont.h"
#include "model_rsa.h"
#include "sc_testbench.h"
#include "verilog/dtype/vint.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace verilog;

using IN = RSAMontModIn;
using OUT = RSAMontModOut;
using DUT = RSAMont;

class ScTestbench_RSAMont : public ScTestbench<IN, OUT, DUT> {
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

int sc_main(int, char **) {
  unique_ptr<ScTestbench_RSAMont> tb =
      CreateScTestbench<ScTestbench_RSAMont>("montgomery");
  KeyType packed, msg, key, modulus, ans;
  from_hex(packed,
           "0AF39E1F831CB4FCD92B17F61F473735C687593A931C97D2B60AD6C7443F09FDB");
  from_hex(msg,
           "412820616369726641206874756F53202C48544542415A494C452054524F50");
  from_hex(key, "10001");
  from_hex(modulus,
           "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1");
  from_hex(ans,
           "0D41B183313D306ADCA09126F3FED6CDEC7DCDCE49DB5C85CB2A37F08C0F2E31");
  tb->push_input({packed, msg, key, modulus});
  tb->push_golden(ans);
  return tb->run(10, SC_US);
}
