#pragma once

#include "callback.h"
#include "dut_wrapper.h"
#include "scoreboard.h"

#include <memory>
#include <systemc>

using namespace std;
using namespace sc_core;

void KillSimulation() {
  wait(100, SC_NS);
  sc_stop();
}

template <typename In, typename Out, typename DUT> SC_MODULE(TestBench) {
public:
  using InType = In;
  using OutType = Out;

  TestBench(const sc_module_name &name)
      : sc_module(name), clk("clk", 1.0, SC_NS), dut_wrapper("dut_wrapper"),
        score_board(new ScoreBoard<OutType>(KillSimulation)) {
    dut_wrapper.clk(clk);

    driver = make_shared<Driver<InType>>(
        dut_wrapper.dut->i_valid, dut_wrapper.dut->i_ready,
        [this](const InType &in) { this->writer(in); });
    monitor = make_shared<Monitor<OutType>>(
        dut_wrapper.dut->o_valid, dut_wrapper.dut->o_ready,
        [this]() { return this->reader(); },
        [this](const OutType &out) { return this->notify(out); },
        KillSimulation);
    dut_wrapper.register_callback(driver);
    dut_wrapper.register_callback(monitor);
  }

  void push_input(const InType &in) { driver->push_back(in); }
  void push_golden(const OutType &out) { score_board->push_golden(out); }
  virtual void notify(const OutType &out) { score_board->push_received(out); }
  virtual void writer(const InType &in) = 0;
  virtual OutType reader() = 0;

protected:
  std::vector<OutType> golden;
  DUTWrapper<DUT> dut_wrapper;
  shared_ptr<Driver<InType>> driver;
  shared_ptr<Monitor<OutType>> monitor;
  unique_ptr<ScoreBoard<OutType>> score_board;

private:
  sc_clock clk;
};