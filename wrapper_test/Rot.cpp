#include "VRotSeq.h"
#include "VRotComb.h"
#include "verilated_vcd_c.h"
#include <cstdint>
#include <iostream>
#include <memory>
using namespace std;

constexpr uint64_t T0 = 19;
static uint64_t timestamp = 0;

struct LogicIn {
	virtual uint64_t Read() = 0;
};

struct LogicOut {
	virtual void Write(uint64_t) = 0;
};

class Logic : public LogicIn, public LogicOut {
	uint64_t v;
public:
	uint64_t Read() override { return v; }
	void Write(uint64_t newv) override { v = newv; }
};

template<typename DUT>
class DUT_crtp {
protected:
	DUT dut;
	VerilatedVcdC tfp;

	void InitDUT(const char* fst_name) {
		dut.trace(&tfp, 99);
		tfp.open(fst_name);
		dut.clk = 0;
		dut.rst = 1;
		for (unsigned i = 0; i <= T0; ++i) {
			tfp.dump(i);
			dut.clk = i&1;
			dut.rst = i < 5 or i > 15;
			dut.eval();
		}
	}

	void EvaluateDUT(bool has_comb_path) {
		// all input for this cycle is used
		// we don't need the value anymore
		// advance the value to the next cycle beforehand
		// this if needed for the terminating Comb,
		// and you can share this part for all wrappers
		if (has_comb_path) {
			dut.eval();
		}
		tfp.dump(timestamp);
		dut.clk = 0;
		dut.eval();
		tfp.dump(timestamp+1);
		dut.clk = 1;
		dut.eval();
	}

	~DUT_crtp() {
		tfp.close();
	}
};

struct RotSeq : private DUT_crtp<VRotSeq> {
	LogicIn* i;
	LogicOut* o;
	uint64_t o_shadow = 0;

	void Init() {
		InitDUT("RotSeq.vcd");
	}

	void Comb1() {
		// C++ -> DUT
		o->Write(o_shadow);

		// DUT -> C++
	}

	void Comb2() {
		// C++ -> DUT
		dut.i = i->Read();

		// DUT
		EvaluateDUT(false);

		// DUT -> C++
	}

	void FF() {
		// Update the register
		// Since we have already entered the next cycle in Comb*
		// We only need to read the DUT's output which is already the next cycle value
		o_shadow = dut.o;
	}
};

struct RotComb : private DUT_crtp<VRotComb> {
	LogicIn* i;
	LogicOut* o;

	void Init() {
		InitDUT("RotComb.vcd");
	}

	void Comb() {
		// C++ -> DUT
		dut.i = i->Read();

		// DUT
		EvaluateDUT(true);

		// DUT -> C++
		o->Write(dut.o);
	}

	void FF() {
		// This module has no register to be updated
	}
};

struct Top {
	RotComb comb;
	RotSeq seq;
	Logic c2s, s2c;
	void Init() {
		comb.i = &s2c;
		seq.o = &s2c;
		comb.o = &c2s;
		seq.i = &c2s;

		comb.Init();
		seq.Init();
	}
	void Comb() {
		// topological order
		seq.Comb1();
		comb.Comb();
		seq.Comb2();
	}
	void FF() {
		// can be any order (non-blocking)
		seq.FF();
		comb.FF();
	}
};

int main() {
	unique_ptr<Top> top(new Top);
	Verilated::traceEverOn(true);

	top->Init();
	for (timestamp = T0+1; timestamp < 120; timestamp += 2) {
		top->FF();
		top->Comb();
	}
	return 0;
}
