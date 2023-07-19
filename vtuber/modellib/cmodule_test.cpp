#include "modellib/cmodule.h"
#include <gtest/gtest.h>
using namespace verilog;

struct ModuleA : public Module<ModuleA> {
	bool triggered = false;
	void ClockComb1() { triggered = true; }
};

struct ModuleB : public Module<ModuleB> {
	ModuleA a;
	bool reset = false;
	bool triggered = false;
	ModuleB() {
		Connect(a, ModuleEventId::kClockComb0, ModuleEventId::kClockComb1);
	}
	void Reset() { reset = true; }
	void ClockComb1() { triggered = true; }
};

TEST(TestModuleLib, Plain) {
	ModuleA a;
	a.NotifySlot(ModuleEventId::kReset);
	EXPECT_FALSE(a.triggered);

	a.NotifySlot(ModuleEventId::kClockComb1);
	EXPECT_TRUE(a.triggered);
}

TEST(TestModuleLib, Nested) {
	ModuleB b;

	b.NotifySlot(ModuleEventId::kReset);
	EXPECT_FALSE(b.a.triggered);
	EXPECT_TRUE(b.reset);
	EXPECT_FALSE(b.triggered);

	b.NotifySlot(ModuleEventId::kClockComb0);
	EXPECT_TRUE(b.a.triggered);
	EXPECT_TRUE(b.reset);
	EXPECT_FALSE(b.triggered);

	b.NotifySlot(ModuleEventId::kClockComb1);
	EXPECT_TRUE(b.a.triggered);
	EXPECT_TRUE(b.reset);
	EXPECT_TRUE(b.triggered);
}
