#include <iostream>
#include <memory>
#include <vector>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include "model_rsa.h"
#include "verilog/dtype/vint.h"
#include "modellib/cport.h"
#include "modellib/cvalidready.h"
#include "modellib/cfifo.h"
#include "modellib/cmodule.h"

using namespace verilog;
using namespace std;

namespace _TwoPower_NSC {
	using ExtendKeyType = verilog::vuint<kBW + 1>;
	struct Stage1To2Type {
		ExtendKeyType modulus;
		IntType power;
	};
};

struct TwoPower_NSC : public Module<TwoPower_NSC> {
private:
	using ExtendKeyType = _TwoPower_NSC::ExtendKeyType;
	using Stage1To2Type = _TwoPower_NSC::Stage1To2Type;

public:
	vrslave<TwoPowerIn> data_in;
private:
	cfifo<Stage1To2Type> fifo_io;
	// this one is a bit stupid, we cannot read fifo_io.out directly
	// must create a pseudo connection instead
	vrslave<Stage1To2Type> fifo_io_rx;
	// stage local registers
	struct {
		bool first;
		IntType counter;
		ExtendKeyType round_result;
	} HandleStage2Local;
public:
	vrmaster<TwoPowerOut> data_out;

	TwoPower_NSC() : fifo_io(1) {
		Connect(fifo_io, verilog::ModuleEventId::kClockComb0, verilog::ModuleEventId::kClockComb0);
		Connect(fifo_io, verilog::ModuleEventId::kClockSeq0, verilog::ModuleEventId::kClockSeq0);

		data_in->write_func = [this](const TwoPowerIn &in) {
			return HandleStage1(in);
		};
		fifo_io_rx(fifo_io.out);
		fifo_io_rx->write_func = [this](const Stage1To2Type &in) {
			return HandleStage2(in);
		};
	}

	void Reset() {
		LOG(INFO) << "Reset";
		auto &ls2 = HandleStage2Local;
		ls2.first = true;
		ls2.counter = 0;
		ls2.round_result = 0;
	}

	bool HandleStage1(const TwoPowerIn &in) {
		if (fifo_io.full()) {
			return false;
		}
		Stage1To2Type data_1to2;
		data_1to2.modulus = static_cast<ExtendKeyType>(in.modulus);
		data_1to2.power = in.power;
		fifo_io.in->write(data_1to2);
		return true;
	}

	bool HandleStage2(const Stage1To2Type &in) {
		auto &local = HandleStage2Local;
		if (local.first) {
			local.first = false;
			local.counter = 0;
			local.round_result = 1;
		}
		if (in.power == local.counter) {
			if (data_out->write(KeyType(local.round_result))) {
				return true;
			}
			local.first = true;
		} else {
			local.round_result <<= 1;
			if (local.round_result > in.modulus) {
				local.round_result -= in.modulus;
			}
			local.counter += 1;
		}
		return false;
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
