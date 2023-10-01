#include "model_rsa.h"
#include "modellib/cfifo.h"
#include "modellib/cmodule.h"
#include "modellib/cport.h"
#include "modellib/cvalidready.h"
#include "verilog/dtype/vint.h"
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <vector>

using namespace verilog;
using namespace std;

namespace _Montgomery_NSC {
  using ExtendKeyType = verilog::vuint<kBW + 2>;
  struct Stage1To2Type {
    KeyType a;
    ExtendKeyType b, modulus;
  };
};

struct Montgomery_NSC : public Module<Montgomery_NSC> {

private:
  using ExtendKeyType = _Montgomery_NSC::ExtendKeyType;
  using Stage1To2Type = _Montgomery_NSC::Stage1To2Type;

public:
  vrslave<MontgomeryIn> data_in;
private:
  cfifo<Stage1To2Type> fifo_io;
  // this one is a bit stupid, we cannot read fifo_io.out directly
  // must create a pseudo connection instead
  vrslave<Stage1To2Type> fifo_io_rx;
public:
  vrmaster<MontgomeryOut> data_out;

  // stage local registers
  struct {
    bool first;
    int idx;
    ExtendKeyType result;
  } HandleStage2Local;

  Montgomery_NSC(): fifo_io(1) {
    Connect(fifo_io, verilog::ModuleEventId::kClockComb0, verilog::ModuleEventId::kClockComb0);
    Connect(fifo_io, verilog::ModuleEventId::kClockSeq0, verilog::ModuleEventId::kClockSeq0);

    data_in->write_func = [this](const MontgomeryIn &in) {
      return HandleStage1(in);
    };
    fifo_io_rx(fifo_io.out);
    fifo_io_rx->write_func = [this](const Stage1To2Type &in) {
      return HandleStage2(in);
    };
  }

  void Reset() {
    auto &ls2 = HandleStage2Local;
    ls2.first = true;
    ls2.result = 0;
    ls2.idx = 0;
  }

  bool HandleStage1(const MontgomeryIn &in) {
    if (fifo_io.full()) {
      return false;
    }
    Stage1To2Type data_1to2;
    data_1to2.a = in.a;
    data_1to2.b = static_cast<ExtendKeyType>(in.b);
    data_1to2.modulus = static_cast<ExtendKeyType>(in.modulus);
    fifo_io.in->write(data_1to2);
    return true;
  }

  bool HandleStage2(const Stage1To2Type &in) {
    auto &local = HandleStage2Local;
    // loop initialization
    if (local.first) {
      local.result = ExtendKeyType(0);
      local.idx = 0;
      local.first = false;
    }
    if (local.idx == kBW) {
      if (data_out->write(KeyType(local.result))) {
        LOG(INFO) << "Sent";
        return true;
      }
      local.first = true;
    } else {
      if (in.a.Bit(local.idx++)) {
        local.result += in.b;
      }
      if (local.result.Bit(0)) {
        local.result += in.modulus;
      }
      local.result >>= 1;
    }
    return false;
  }
};

struct Montgomery_TB : public Module<Montgomery_TB> {
  vrmaster<MontgomeryIn> data_in;
  vrslave<MontgomeryOut> data_out;
  Montgomery_NSC dut;

  MontgomeryIn testdata_in;
  vector<MontgomeryOut> testdata_out;
  MontgomeryOut testdata_gold;

  unsigned sent, received;
  bool finished;

  Montgomery_TB() {
    sent = 0;
    received = 0;
    finished = false;

    // create data
    const char str_N[] =
        "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1";
    const char str_A[] =
        "412820616369726641206874756F53202C48544542415A494C452054524F50";
    const char str_B[] = "10001";
    const char str_ans[] =
        "314F8ACB18E57C4B2FA37ADEFA7964711B8DCDB7AAC7514C78D97CF4D4121426";
    from_hex(testdata_in.modulus, str_N);
    from_hex(testdata_in.a, str_A);
    from_hex(testdata_in.b, str_B);
    from_hex(testdata_gold, str_ans);

    // connect submodule
    data_in(dut.data_in);
    data_out(dut.data_out);
    data_out->write_func = [this](const MontgomeryOut &x) {
      ++received;
      testdata_out.push_back(x);
      return true;
    };
    Connect(dut, ModuleEventId::kReset, ModuleEventId::kReset);
    Connect(dut, ModuleEventId::kClockComb0, ModuleEventId::kClockComb0);
    Connect(dut, ModuleEventId::kClockSeq0, ModuleEventId::kClockSeq0);
  }

  void ClockComb0() {
    if (sent == 0) {
      if (data_in->write(testdata_in)) {
        ++sent;
      }
    }
  }

  void ClockSeq0() {
    if (received != 0) {
      finished = true;
    }
  }

  void Verify() {
    ASSERT_EQ(testdata_out.size(), 1);
    EXPECT_EQ(testdata_out[0], testdata_gold);
  }
};

TEST(NoSysC, Montgomery) {
  unique_ptr<Montgomery_TB> tb(new Montgomery_TB);

  // simulate
  const unsigned kTimeout = 1000, kRemain = 10;
  unsigned i, stop_at = kTimeout;
  tb->NotifySlot(ModuleEventId::kReset);
  for (i = 0; i < stop_at; ++i) {
    tb->NotifySlot(ModuleEventId::kClockComb0);
    tb->NotifySlot(ModuleEventId::kClockSeq0);
    if (tb->finished) {
      stop_at = i + kRemain;
      break;
    }
  }
  ASSERT_NE(i, kTimeout) << "Timeout";
  for (; i <= stop_at; ++i) {
    tb->NotifySlot(ModuleEventId::kClockComb0);
    tb->NotifySlot(ModuleEventId::kClockSeq0);
  }
  LOG(INFO) << "Done @" << i;
  tb->Verify();
}
