#pragma once

#include "abstract_random.h"
#include "input_connector.h"
#include "output_connector.h"
#include "dut_wrapper.h"
#include "scoreboard.h"

#include <glog/logging.h>
#include <memory>
#include <systemc>

using namespace std;
using namespace sc_core;

void KillSimulation() {
  wait(100, SC_NS);
  LOG(ERROR) << "Kill simulation at " << sc_core::sc_time_stamp() << endl;
  sc_stop();
}

template <typename In, typename Out, typename DUT> SC_MODULE(TestBench) {
public:
  using InType = In;
  using OutType = Out;

  TestBench(const sc_module_name &name, bool dump_waveform = false,
            BoolPattern *driver_random_policy = nullptr,
            BoolPattern *monitor_random_policy = nullptr)
      : sc_module(name), dut_wrapper("dut_wrapper", dump_waveform),
        score_board(new ScoreBoard<OutType>(KillSimulation)),
        clk("clk", 1.0, SC_NS) {
    dut_wrapper.clk(clk);

    driver = make_shared<InputConnector<InType>>(
        dut_wrapper.dut->i_valid, dut_wrapper.dut->i_ready,
        [this](const InType &in) { this->writer(in); }, driver_random_policy);
    monitor = make_shared<OutputConnector<OutType>>(
        dut_wrapper.dut->o_valid, dut_wrapper.dut->o_ready,
        [this]() { return this->reader(); },
        [this](const OutType &out) { return this->notify(out); },
        KillSimulation, monitor_random_policy);
    dut_wrapper.register_connector(driver);
    dut_wrapper.register_connector(monitor);
  }

  int run(int duration, sc_time_unit unit) {
    sc_start(duration, unit);
    if (!score_board->is_pass()) {
      LOG(ERROR) << "Score board result mismatch" << endl;
    }
    return !score_board->is_pass();
  }

  void push_input(const InType &in) { driver->push_back(in); }
  void push_golden(const OutType &out) { score_board->push_golden(out); }
  virtual void notify(const OutType &out) { score_board->push_received(out); }
  virtual void writer(const InType &in) = 0;
  virtual OutType reader() = 0;

protected:
  std::vector<OutType> golden;
  DUTWrapper<DUT> dut_wrapper;
  shared_ptr<InputConnector<InType>> driver;
  shared_ptr<OutputConnector<OutType>> monitor;
  unique_ptr<ScoreBoard<OutType>> score_board;

private:
  sc_clock clk;
};