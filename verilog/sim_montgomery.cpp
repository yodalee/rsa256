
#include "Vmontgomery.h"
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

using DUT = Vmontgomery;

void KillSimulation() {
  wait(100, SC_NS);
  sc_stop();
}

SC_MODULE(Testbench) {
  sc_clock clk;
  DUTWrapper<DUT> dut_wrapper;
  shared_ptr<Driver<RSAMontgomeryModIn>> driver;
  shared_ptr<Monitor<RSAMontgomeryModOut>> monitor;
  unique_ptr<ScoreBoard<RSAMontgomeryModOut>> score_board;

public:
  SC_HAS_PROCESS(Testbench);
  Testbench(const sc_module_name &name)
      : sc_module(name), clk("clk", 1.0, SC_NS), dut_wrapper("dut_wrapper"),
        score_board(new ScoreBoard<RSAMontgomeryModOut>(KillSimulation)) {
    dut_wrapper.clk(clk);
    RSAMontgomeryModOut golden;
    from_hex(
        golden,
        "314F8ACB18E57C4B2FA37ADEFA7964711B8DCDB7AAC7514C78D97CF4D4121426");

    driver = make_shared<Driver<RSAMontgomeryModIn>>(
        dut_wrapper.dut->i_valid, dut_wrapper.dut->i_ready,
        [this](const RSAMontgomeryModIn &in) { this->writer(in); });
    monitor = make_shared<Monitor<RSAMontgomeryModOut>>(
        dut_wrapper.dut->o_valid, dut_wrapper.dut->o_ready,
        [this]() { return this->reader(); },
        [this](const RSAMontgomeryModOut &out) { return this->notify(out); },
        KillSimulation);
    dut_wrapper.register_callback(driver);
    dut_wrapper.register_callback(monitor);

    KeyType a;
    KeyType b;
    KeyType modulus;
    from_hex(a,
             "412820616369726641206874756F53202C48544542415A494C452054524F50");
    from_hex(b, "10001");
    from_hex(
        modulus,
        "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1");
    driver->push_back({.a = a, .b = b, .modulus = modulus});
  }

  void notify(const RSAMontgomeryModOut &out) {
    score_board->push_received(out);
  }

  void writer(const RSAMontgomeryModIn &in) {
    write_verilator_port(dut_wrapper.dut->i_a, in.a);
    write_verilator_port(dut_wrapper.dut->i_b, in.b);
    write_verilator_port(dut_wrapper.dut->i_modulus, in.modulus);
  }

  RSAMontgomeryModOut reader() {
    RSAMontgomeryModOut out;
    read_verilator_port(out, dut_wrapper.dut->o_out);
    std::cout << "Received: " << out << std::endl;
    return out;
  }
};

int sc_main(int, char **) {
  unique_ptr<Testbench> testbench(new Testbench("testbench"));
  sc_start(1200.0, SC_NS);
  return 0;
}