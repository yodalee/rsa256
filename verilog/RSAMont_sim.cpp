
#include "VRSAMont.h"
#include "bridge/verilator/testbench.h"
#include "bridge/verilator/verilator_assign.h"
#include "model_rsa.h"
#include "source.h"
#include "verilog/dtype.h"
#include <fstream>
#include <iostream>
#include <systemc>

using namespace std;
using namespace sc_core;
using namespace verilog::verilator;

using IN = RSAMontModIn;
using DUT = VRSAMont;
using OUT = RSAMontModOut;

class TestBench_RsaMont : public TestBench<IN, OUT, DUT> {
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
  unique_ptr<TestBench_RsaMont> testbench(
      new TestBench_RsaMont("testbench_rsamont_sv", /*dump=*/true));

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

  ifstream ifm("RSA_m.in");
  VintHexSource<KeyType> source_m;
  auto v_m = source_m.get(ifm);
  ifstream ifc("RSA_c.in");
  VintHexSource<RSAModOut> source_c;
  auto v_c = source_c.get(ifc);

  // sample in
  RSAMontModIn in;
  from_hex(in.base,
           "0AF39E1F831CB4FCD92B17F61F473735C687593A931C97D2B60AD6C7443F09FDB");
  from_hex(in.key, "10001");
  from_hex(in.modulus,
           "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1");
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
