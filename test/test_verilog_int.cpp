#include <gtest/gtest.h>

#include "verilog_int.h"

TEST(TestVerilogInt, FromHex) {
	verilog::vint<false, 8> v8;
	from_hex(v8, "2A");
	EXPECT_EQ(v8.v[0], 0x2A);

	verilog::vint<false, 12> v12;
	from_hex(v12, "2A2A");
	EXPECT_EQ(v12.v[0], 0xA2A);

	verilog::vint<false, 128> v128;
	from_hex(v128, "2A2A");
	EXPECT_EQ(v128.v[0], 0x2A2A);
	EXPECT_EQ(v128.v[1], 0);
}

TEST(TestVerilogInt, ToHex) {
	verilog::vint<false, 8> v8;
	v8.v[0] = 0x2A;
	EXPECT_EQ(to_hex(v8), "2a");

	verilog::vint<false, 12> v12;
	v12.v[0] = 0xA2A;
	EXPECT_EQ(to_hex(v12), "a2a");
}
