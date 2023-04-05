
#include "VRSA.h"
#include "bridge/verilator/testbench.h"
#include "bridge/verilator/verilator_assign.h"
#include "model_rsa.h"
#include "source.h"
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
};

class Driver : public InputConnector<IN, DUT> {
public:
  using InputConnector::InputConnector;
  void write(DUT *dut, const IN &in) override {
    write_port(dut->i_in, verilog::pack(in));
  }
};

class Monitor : public OutputConnector<OUT, DUT> {
public:
  using OutputConnector::OutputConnector;
  OUT read(const DUT *dut) override {
    OUT out;
    read_port(out, dut->o_out);
    return out;
  }
};

const char str_key[] = "10001";
const char str_modulus[] =
    "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1";

int sc_main(int, char **) {
  unique_ptr<TestBench_Rsa> testbench(
      new TestBench_Rsa("testbench_rsa_sv", /*dump=*/true));

  auto driver =
      make_shared<Driver>(testbench->dut_wrapper.dut->i_valid,
                          testbench->dut_wrapper.dut->i_ready, nullptr);
  auto monitor = make_shared<Monitor>(
      testbench->dut_wrapper.dut->o_valid, testbench->dut_wrapper.dut->o_ready,
      [&](const OUT &out) { return testbench->notify(out); }, KillSimulation,
      nullptr);
  testbench->register_connector(
      static_cast<shared_ptr<Connector<DUT>>>(driver));
  testbench->register_connector(
      static_cast<shared_ptr<Connector<DUT>>>(monitor));

  VintLineSource<KeyType> source_m("RSA_m.in");
  auto v_m = source_m.get();
  VintLineSource<RSAModOut> source_c("RSA_c.in");
  auto v_c = source_c.get();

  // sample in
  RSAModIn in;
  from_hex(in.key, str_key);
  from_hex(in.modulus, str_modulus);
  for (const auto &m : v_m) {
    in.msg = m;
    driver->push_back(in);
  }
  // sample out
  for (const auto &c : v_c) {
    testbench->push_golden(c);
  }
  return testbench->run(400, SC_US);
}
