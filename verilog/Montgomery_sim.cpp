
#include "VMontgomery.h"
#include "bridge/verilator/testbench.h"
#include "bridge/verilator/verilator_assign.h"
#include "model_rsa.h"
#include "verilog/dtype.h"
#include <iostream>
#include <systemc>

using namespace std;
using namespace sc_core;
using namespace verilog::verilator;

using IN = MontgomeryIn;
using DUT = VMontgomery;
using OUT = MontgomeryOut;

class TestBench_Montgomery : public TestBench<IN, OUT, DUT> {
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

int sc_main(int, char **) {
  unique_ptr<TestBench_Montgomery> testbench(
      new TestBench_Montgomery("testbench_montgomery_sv", /*dump=*/true));

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

  KeyType a("6C7DA0B0F70C7D3DFE4EF30ECA54694E68E7AD2A7BD63EDBB71C77B178BEEEF4");
  KeyType b("6C7DA0B0F70C7D3DFE4EF30ECA54694E68E7AD2A7BD63EDBB71C77B178BEEEF4");
  KeyType modulus(
      "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1");
  driver->push_back({.a = a, .b = b, .modulus = modulus});
  from_hex(a, "412820616369726641206874756F53202C48544542415A494C452054524F50");
  from_hex(b, "10001");
  driver->push_back({.a = a, .b = b, .modulus = modulus});

  TestBench_Montgomery::OutType golden(
      "1ECC89942DF6DD65E01D20F2AC49F495CB47F0EA9977351FD92DD3F8FD4B33D7");
  testbench->push_golden(golden);
  from_hex(golden,
           "314f8acb18e57c4b2fa37adefa7964711b8dcdb7aac7514c78d97cf4d4121426");
  testbench->push_golden(golden);

  return testbench->run(750, SC_NS);
}
