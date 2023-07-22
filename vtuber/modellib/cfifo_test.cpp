#include "modellib/cmodule.h"
#include "modellib/cfifo.h"
#include <gtest/gtest.h>
#include <iostream>
using namespace verilog;
using namespace std;

struct ModuleA : public Module<ModuleA> {
	cfifo_out<int> fout;
	int counter = 0;
	ModuleA() {
	}
	void ClockComb0() {
		if (fout->full()) {
			return;
		}
		fout->write(counter++);
	}
};

struct ModuleB : public Module<ModuleB> {
	cfifo_in<int> fin;
	cfifo_out<int> fout;
	unsigned cooldown = 0;
	ModuleB() {
	}
	void ClockComb0() {
		if (cooldown != 0) {
			--cooldown;
			return;
		}
		if (fin->empty()) {
			return;
		}
		cooldown += 2;
		if (fin->read() == 10) {
			const bool out_ok = not fout->full();
			ASSERT_TRUE(out_ok);
			fout->write(5566);
		}
	}
};

struct ModuleC : public Module<ModuleC> {
	ModuleA ma;
	ModuleB mb;
	cfifo<int> fifo_ab;
	cfifo_out_export<int> fout;
	ModuleC() : fifo_ab(2) {
		Connect(ma, ModuleEventId::kClockComb0, ModuleEventId::kClockComb0);
		Connect(ma, ModuleEventId::kClockSeq0, ModuleEventId::kClockSeq0);
		Connect(mb, ModuleEventId::kClockComb0, ModuleEventId::kClockComb0);
		Connect(mb, ModuleEventId::kClockSeq0, ModuleEventId::kClockSeq0);
		Connect(fifo_ab, ModuleEventId::kClockComb0, ModuleEventId::kClockComb0);
		Connect(fifo_ab, ModuleEventId::kClockSeq0, ModuleEventId::kClockSeq0);
		// Connect ports
		ma.fout = &fifo_ab;
		mb.fin = &fifo_ab;
		// Export ports
		fout = &mb.fout;
	}
};

struct Testbench : public Module<Testbench> {
	ModuleC mc;
	cfifo<int> fifo_c;
	bool done = false;
	Testbench() : fifo_c(10) {
		Connect(mc, ModuleEventId::kClockComb0, ModuleEventId::kClockComb0);
		Connect(mc, ModuleEventId::kClockSeq0, ModuleEventId::kClockSeq0);
		Connect(fifo_c, ModuleEventId::kClockComb0, ModuleEventId::kClockComb0);
		Connect(fifo_c, ModuleEventId::kClockSeq0, ModuleEventId::kClockSeq0);
		(*mc.fout) = &fifo_c;
	}
	void ClockComb0() {
		if (fifo_c.empty()) {
			return;
		}
		const int gold = fifo_c.read();
		done = true;
		EXPECT_EQ(gold, 5566);
	}
};

TEST(TestFifo, Test1) {
	Testbench tb;

	tb.NotifySlot(ModuleEventId::kReset);
	unsigned c;
	for (c = 0; c < 100; ++c) {
		tb.NotifySlot(ModuleEventId::kClockComb0);
		tb.NotifySlot(ModuleEventId::kClockSeq0);
		if (tb.done) {
			break;
		}
	}
	EXPECT_NE(c, 1000) << "Simulation too long";
	// 1 cycle a->b
	// 10*(cooldown+1) cycles
	// 1 cycle b->done
	EXPECT_EQ(c, 32);
}
