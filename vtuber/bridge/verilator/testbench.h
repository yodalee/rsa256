#pragma once

#include "abstract_random.h"
#include "dut_wrapper.h"
#include "input_connector.h"
#include "output_connector.h"
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

  TestBench(const sc_module_name &name, bool dump_waveform = false)
      : sc_module(name), dut_wrapper("dut_wrapper", dump_waveform),
        score_board(new ScoreBoard<OutType>(KillSimulation)),
        clk("clk", 1.0, SC_NS) {
    dut_wrapper.clk(clk);
  }

  int run(int duration, sc_time_unit unit) {
    sc_start(duration, unit);
    if (!score_board->is_pass()) {
      LOG(ERROR) << "Score board result mismatch" << endl;
    }
    return !score_board->is_pass();
  }

  void register_connector(shared_ptr<Connector<DUT>> connector) {
    dut_wrapper.register_connector(connector);
  }
  void push_golden(const OutType &out) { score_board->push_golden(out); }
  virtual void notify(const OutType &out) { score_board->push_received(out); }

  DUTWrapper<DUT> dut_wrapper;

protected:
  std::vector<OutType> golden;
  unique_ptr<ScoreBoard<OutType>> score_board;

private:
  sc_clock clk;
};