#include "model_rsa.h"
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

struct Montgomery_NSC : public Module<Montgomery_NSC> {
  using ExtendKeyType = verilog::vuint<kBW + 2>;

  vrslave<MontgomeryIn> data_in;
  vrmaster<MontgomeryOut> data_out;

  KeyType a_r;
  ExtendKeyType b_r, result_r, modulus_r;
  int idx = 0;
  bool running_r, running_up_w, running_down_w;

  Montgomery_NSC() {
    data_in->write_func = [this](const MontgomeryIn &in) {
      if (running_r or running_up_w) {
        return false;
      }
      running_up_w = true;
      a_r = in.a;
      b_r = static_cast<ExtendKeyType>(in.b);
      modulus_r = static_cast<ExtendKeyType>(in.modulus);
      result_r = ExtendKeyType(0);
      idx = 0;
      return true;
    };
  }

  void Reset() {
    LOG(INFO) << "Reset";
    running_r = false;
    a_r = 0;
    b_r = 0;
    result_r = 0;
    modulus_r = 0;
    idx = 0;
  }

  void ClockComb0() {
    if (not running_r) {
      return;
    }

    if (idx == kBW) {
      if (data_out->write(KeyType(result_r))) {
        LOG(INFO) << "Sent";
        running_down_w = true;
      }
    } else {
      if (a_r.Bit(idx++)) {
        result_r += b_r;
      }
      if (result_r.Bit(0)) {
        result_r += modulus_r;
      }
      result_r >>= 1;
    }
  }

  void ClockSeq0() {
    CHECK(not(running_up_w and running_down_w));
    if (running_up_w) {
      LOG(INFO) << "up";
      running_r = true;
    } else if (running_down_w) {
      running_r = false;
    }
    running_up_w = false;
    running_down_w = false;
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
