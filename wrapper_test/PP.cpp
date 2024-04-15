#include "VPPFast.h"
#include "VPPSlow.h"
#include "verilated_fst_c.h"
#include <memory>
#include <cstdint>
using namespace std;

constexpr uint64_t T0 = 21;
static uint64_t timestamp = 0;

struct HandshakeIn {
	virtual bool CanRead() = 0;
	virtual uint64_t Read() = 0;
	virtual uint64_t Peek() = 0;
	void Drop() { (void)Read(); }
};

struct HandshakeOut {
	virtual bool CanWrite() = 0;
	virtual void Write(uint64_t) = 0;
};

class Handshake : public HandshakeIn, public HandshakeOut {
	uint64_t v;
	bool valid = false, en = false;
public:
	bool CanRead() override { return valid; }
	uint64_t Read() override { valid = false; return v; }
	uint64_t Peek() override { return v; }
	bool CanWrite() override { return not (valid or en); }
	void Write(uint64_t newv) override { en = true; v = newv; }
	void FF() { if (en) { valid = 1; en = 0; } }
};

template<typename DUT>
class DUT_crtp {
protected:
	DUT dut;
	VerilatedFstC tfp;

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

struct PPFast : DUT_crtp<VPPFast> {
	HandshakeIn *i;
	HandshakeOut *o;

	void Init() {
		InitDUT("PPFast.fst");
	}
	void Comb() {
		// C++ -> DUT
		bool oready = o->CanWrite();
		dut.oready = oready;

		// DUT
		EvaluateDUT(true);

		// DUT -> C++
		bool iready = dut.iready;
		dut.ivalid = i->CanRead();
		if (iready and dut.ivalid) {
			dut.idata = i->Read();
		}

		if (oready and dut.ovalid) {
			o->Write(dut.odata);
		}
	}
	void FF() {
	}
};

struct PPSlow : DUT_crtp<VPPSlow> {
	HandshakeIn *i;
	HandshakeOut *o;
	bool iready_shadow = false;

	void Init() {
		dut.trace(&tfp, 99);
		tfp.open("PPSlow.fst");
		dut.rst = 1;
		for (unsigned i = 0; i <= T0; ++i) {
			tfp.dump(i);
			dut.clk = i&1;
			dut.rst = i < 5 or i > 15;
			dut.eval();
		}
	}
	void Comb1() {
		// C++ -> DUT
		bool iready = iready_shadow;
		dut.ivalid = i->CanRead();
		if (iready and dut.ivalid) {
			dut.idata = i->Read();
		}
	}
	void Comb2() {
		// C++ -> DUT
		bool oready = o->CanWrite();
		dut.oready = oready;

		// DUT
		EvaluateDUT(false);

		// DUT -> C++
		if (oready and dut.ovalid) {
			o->Write(dut.odata);
		}
	}
	void FF() {
		// Update the register
		// Since we have already entered the next cycle in Comb*
		// We only need to read the DUT's output which is already the next cycle value
		iready_shadow = dut.iready;
	}
};

struct Top {
	PPSlow slow;
	PPFast fast;
	Handshake s2f, f2s;

	void Init() {
		slow.o = &s2f;
		fast.i = &s2f;

		fast.o = &f2s;
		slow.i = &f2s;

		fast.Init();
		slow.Init();
	}
	void Comb() {
		// topological order
		slow.Comb1();
		fast.Comb();
		slow.Comb2();
	}
	void FF() {
		// can be any order (non-blocking)
		f2s.FF();
		slow.FF();
		s2f.FF();
		fast.FF();
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
