
#include "VTwoPower.h"
#include "bridge/verilator/testbench.h"
#include "bridge/verilator/verilator_assign.h"
#include "model_rsa.h"
#include "verilog/dtype.h"
#include <iostream>
#include <systemc>

using namespace std;
using namespace sc_core;
using namespace verilog::verilator;

using IN = TwoPowerIn;
using DUT = VTwoPower;
using OUT = TwoPowerOut;

class TestBench_TwoPower : public TestBench<IN, OUT, DUT> {
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
  unique_ptr<TestBench_TwoPower> testbench(
      new TestBench_TwoPower("TestBench_two_power_mod_sv", /*dump=*/true));

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

  KeyType modulus(
      "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1");
  driver->push_back({.power = TwoPowerIn::IntType(512), .modulus = modulus});
  driver->push_back({.power = TwoPowerIn::IntType(256), .modulus = modulus});

  TestBench_TwoPower::OutType golden(
      "0AF39E1F831CB4FCD92B17F61F473735C687593A931C97D2B60AD6C7443F09FDB");
  testbench->push_golden(golden);
  from_hex(
      golden,
      "0x1f8edd0d5b5617eebe521ae75d328a8b23498f9f4ffa1db99a10acd1f3358c1f");
  testbench->push_golden(golden);

  return testbench->run(1, SC_US);
}
