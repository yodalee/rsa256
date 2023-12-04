#pragma once

#include "modellib/cmodule.h"
#include "modellib/cvalidready.h"
#include <cstring>
#include <vector>

namespace verilog {

template <typename Dut, typename In, typename Out>
struct CTestbench : public Module<CTestbench<Dut, In, Out>> {
  vrmaster<In> data_in;
  vrslave<Out> data_out;
  Dut dut;

  ::std::vector<In> testdata_in;
  ::std::vector<Out> testdata_out;
  ::std::vector<Out> testdata_gold;

  unsigned sent, received;
  bool finished;

  CTestbench() {
    sent = 0;
    received = 0;
    finished = false;

    // connect submodule
    data_in(dut.data_in);
    data_out(dut.data_out);
    data_out->write_func = [this](const TwoPowerOut &x) {
      ++received;
      testdata_out.push_back(x);
      return true;
    };
    this->Connect(dut, ModuleEventId::kReset, ModuleEventId::kReset);
    this->Connect(dut, ModuleEventId::kClockComb0, ModuleEventId::kClockComb0);
    this->Connect(dut, ModuleEventId::kClockSeq0, ModuleEventId::kClockSeq0);
  }

  void ClockComb0() {
    if (sent < testdata_in.size()) {
      if (data_in->write(testdata_in[sent])) {
        ++sent;
      }
    }
  }

  void ClockSeq0() {
    if (received == testdata_gold.size()) {
      finished = true;
    }
  }

  bool IsPass() {
    if (testdata_out.size() != testdata_gold.size()) {
      return false;
    }
    for (size_t i = 0; i < testdata_gold.size(); i++) {
      auto &out = testdata_out[i];
      auto &gold = testdata_gold[i];
      if (memcmp(&out, &gold, sizeof(Out) != 0)) {
        return false;
      }
    }

    return true;
  };
};

} // namespace verilog
