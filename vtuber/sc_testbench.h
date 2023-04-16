#pragma once

#include "scoreboard.h"
#include "verilog/serialize.h"

#include <fstream>
#include <glog/logging.h>
#include <memory>
#include <systemc>
#include <vector>

using namespace std;
using namespace sc_core;

static inline void KillSimulation() {
  wait(SC_ZERO_TIME);
  LOG(ERROR) << "Kill simulation at " << sc_core::sc_time_stamp() << endl;
  sc_stop();
}

template <typename In, typename Out, typename DUT> SC_MODULE(ScTestbench) {
public:
  using InType = In;
  using OutType = Out;

  SC_HAS_PROCESS(ScTestbench);
  ScTestbench(const sc_module_name &name, const bool dump_)
      : sc_module(name), clk("clk", 1, SC_NS), dut("dut"),
        score_board(new ScoreBoard<OutType>(&KillSimulation)), dump(dump_) {
    dut.clk(clk);
    SC_THREAD(InputThread);
    SC_THREAD(OutputThread);
  }

  void push_golden(const OutType &out) {
    if (dump) {
      data_out.push_back(out);
    }
    score_board->push_golden(out);
  }
  void push_input(const InType &in) { data_in.push_back(in); }

  void end_of_elaboration() override {
    if (!dump) {
      return;
    }

    // dump input and output to file "module_name.in" and "module_name.out"
    std::fstream f_data_in, f_data_out;
    std::string module_name(name());

    f_data_in.open(module_name + ".in", std::fstream::out);
    for (const auto &in : data_in) {
      SaveContent(f_data_in, in);
    }
    f_data_in.close();

    f_data_out.open(module_name + ".out", std::fstream::out);
    for (const auto &out : data_out) {
      SaveContent(f_data_out, out);
    }
    f_data_out.close();
  }

  int run(int duration, sc_time_unit unit) {
    sc_start(duration, unit);
    if (!score_board->is_pass()) {
      LOG(ERROR) << "Score board result mismatch" << endl;
    }
    return !score_board->is_pass();
  }

  void InputThread() {
    for (const auto &in : data_in) {
      WriteInput(in);
    }
  }

  void OutputThread() {
    while (true) {
      const auto &out = ReadOutput();
      score_board->push_received(out);
    }
  }

  virtual void Connect() = 0;
  virtual void WriteInput(const InType &in) = 0;
  virtual OutType ReadOutput() = 0;

protected:
  sc_clock clk;
  std::vector<InType> data_in;
  std::vector<OutType> data_out;
  DUT dut;
  unique_ptr<ScoreBoard<OutType>> score_board;
  bool dump;
};

template <typename TB>
unique_ptr<TB> CreateScTestbench(const sc_module_name &name,
                                 const bool dump = false) {
  unique_ptr<TB> tb{new TB(name, dump)};
  tb->Connect();
  return tb;
}