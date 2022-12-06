
#include "Vtwo_power_mod.h"
#include "assign_port.h"
#include "callback.h"
#include "dut_wrapper.h"
#include "model_rsa.h"
#include "scoreboard.h"
#include "verilog_int.h"
#include <iostream>
#include <systemc>

using namespace std;
using namespace sc_core;

using DUT = Vtwo_power_mod;

void KillSimulation() {
  wait(100, SC_NS);
  sc_stop();
}

SC_MODULE(Testbench) {
  sc_clock clk;
  DUTWrapper<DUT> dut_wrapper;
  shared_ptr<Driver<RSATwoPowerModIn>> driver;
  shared_ptr<Monitor<RSATwoPowerModOut>> monitor;
  unique_ptr<ScoreBoard<RSATwoPowerModOut>> score_board;

public:
  SC_HAS_PROCESS(Testbench);
  Testbench(const sc_module_name &name)
      : sc_module(name), clk("clk", 1.0, SC_NS), dut_wrapper("dut_wrapper"),
        score_board(new ScoreBoard<RSATwoPowerModOut>(KillSimulation)) {
    dut_wrapper.clk(clk);
    RSATwoPowerModOut golden;
    from_hex(
        golden,
        "0AF39E1F831CB4FCD92B17F61F473735C687593A931C97D2B60AD6C7443F09FDB");
    score_board->push_golden(golden);

    driver = make_shared<Driver<RSATwoPowerModIn>>(
        dut_wrapper.dut->i_valid, dut_wrapper.dut->i_ready,
        [this](const RSATwoPowerModIn &in) { this->writer(in); });
    monitor = make_shared<Monitor<RSATwoPowerModOut>>(
        dut_wrapper.dut->o_valid, dut_wrapper.dut->o_ready,
        [this]() { return this->reader(); },
        [this](const RSATwoPowerModOut &out) { return this->notify(out); },
        KillSimulation);
    dut_wrapper.register_callback(driver);
    dut_wrapper.register_callback(monitor);

    KeyType modulus;
    from_hex(
        modulus,
        "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1");
    driver->push_back({.power = verilog::vuint<32>(512), .modulus = modulus});
  }

  void notify(const RSATwoPowerModOut &out) { score_board->push_received(out); }

  void writer(const RSATwoPowerModIn &in) {
    auto power = in.power;
    auto modulus = in.modulus;
    std::cout << to_hex(modulus) << std::endl;
    write_verilator_port(dut_wrapper.dut->i_power, power);
    write_verilator_port(dut_wrapper.dut->i_modulus, modulus);
  }

  RSATwoPowerModOut reader() {
    RSATwoPowerModOut out;
    read_verilator_port(out, dut_wrapper.dut->o_out);
    return out;
  }
};

int sc_main(int, char **) {
  unique_ptr<Testbench> testbench(new Testbench("testbench"));
  sc_start(1200.0, SC_NS);
  return 0;
}
