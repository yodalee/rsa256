#include <iostream>
#include <memory>
#include <vector>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include "model_rsa.h"
#include "verilog/dtype/vint.h"
#include "modellib/cport.h"
#include "modellib/cvalidready.h"
#include "modellib/cmodule.h"

using namespace verilog;
using namespace std;

struct TwoPower_NSC : public Module<TwoPower_NSC> {
	vrslave<TwoPowerIn> data_in;
	vrmaster<TwoPowerOut> data_out;

	ExtendKeyType round_result_r, modulus_r;
	IntType power_r;
	bool running_r, running_up_w, running_down_w;

	TwoPower_NSC() {
		data_in->write_func = [this](const TwoPowerIn& x) {
			if (running_r or running_up_w) {
				return false;
			}
			running_up_w = true;
			power_r = x.power;
			modulus_r = ExtendKeyType(x.modulus);
			round_result_r = ExtendKeyType(1);
			return true;
		};
	}

	void ClockComb0() {
		if (not running_r) {
			return;
		}
		if (bool(power_r)) {
			LOG(INFO) << "--";
			round_result_r <<= 1;
			if (round_result_r > modulus_r) {
				round_result_r -= modulus_r;
			}
			power_r -= 1;
		} else {
			if (data_out->write(KeyType(round_result_r))) {
				LOG(INFO) << "Sent";
				running_down_w = true;
			}
		}
	}

	void ClockSeq0() {
		CHECK(not (running_up_w and running_down_w));
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

struct TwoPower_TB : public Module<TwoPower_TB> {
	vrmaster<TwoPowerIn> data_in;
	vrslave<TwoPowerOut> data_out;
	TwoPower_NSC dut;

	TwoPowerIn testdata_in;
	vector<TwoPowerOut> testdata_out;
	TwoPowerOut testdata_gold;

	unsigned sent, received;
	bool finished;

	TwoPower_TB() {
		sent = 0;
		received = 0;
		finished = false;

		// create data
		const char str_modulus[] =
			"0xE07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1";
		const char str_ans[] =
			"AF39E1F831CB4FCD92B17F61F473735C687593A931C97D2B60AD6C7443F09FDB";
		testdata_in.power = 512;
		from_hex(testdata_in.modulus, str_modulus);
		from_hex(testdata_gold, str_ans);

		// connect submodule
		data_in(dut.data_in);
		data_out(dut.data_out);
		data_out->write_func = [this](const TwoPowerOut& x) {
			++received;
			testdata_out.push_back(x);
			return true;
		};
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

TEST(NoSysC, TwoPower) {
	unique_ptr<TwoPower_TB> tb(new TwoPower_TB);

	// simulate
	const unsigned kTimeout = 1000, kRemain = 10;
	unsigned i, stop_at = kTimeout;
	tb->NotifySlot(ModuleEventId::kReset);
	for (i = 0; i < stop_at; ++i) {
		tb->NotifySlot(ModuleEventId::kClockComb0);
		tb->NotifySlot(ModuleEventId::kClockSeq0);
		if (tb->finished) {
			stop_at = i+kRemain;
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
