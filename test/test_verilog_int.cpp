#include "verilog_int.h"
#include <gtest/gtest.h>
#include <string>
using namespace std;
using namespace verilog;

TEST(TestVerilogUnsigned, FromHex) {
	vuint<8> v8;
	from_hex(v8, "2A");
	EXPECT_EQ(v8, 0x2A);

	vuint<13> v13;
	from_hex(v13, "1aC");
	EXPECT_EQ(v13, 0x1ac);
	from_hex(v13, "");
	EXPECT_EQ(v13, 0);
	from_hex(v13, "1..2..3..4");
	EXPECT_EQ(v13, 0x1234);
	from_hex(v13, "ffff");
	EXPECT_EQ(v13, 0x1fff);
	from_hex(v13, "00000f");
	EXPECT_EQ(v13, 0xf);

	vuint<127> a127, b127;
	from_hex(a127, "8123_4567_acac_acac_89AB_cdef_0000_5555");
	from_hex(b127, "00000000 8123 4567 acac acac 89AB cdef 0000 5555");
	EXPECT_EQ(a127.v[1], 0x01234567acacacacllu);
	EXPECT_EQ(a127.v[0], 0x89ABCDEF00005555llu);
	EXPECT_EQ(b127.v[1], 0x01234567acacacacllu);
	EXPECT_EQ(b127.v[0], 0x89ABCDEF00005555llu);
	EXPECT_EQ(a127, b127);
}

TEST(TestVerilogUnsigned, ToHex) {
	vuint<8> v8;
	v8.v[0] = 0x2A;
	EXPECT_EQ(to_hex(v8), "2A");

	vuint<12> v12;
	v12.v[0] = 0xA2A;
	EXPECT_EQ(to_hex(v12), "A2A");

	vuint<66> v66;
	v66.v[1] = 0x2;
	v66.v[0] = 0x01234567890abcdefllu;
	EXPECT_EQ(to_hex(v12), "A2A");

}

TEST(TestVerilogUnsigned, AddSubMulDiv) {
	vuint<10> a10, b10;
	a10.v[0] = 1000;
	b10.v[0] = 124;
	EXPECT_EQ((a10+b10), 100);

	vuint<127> a127, b127;
	a127.v[1] = 1;
	a127.v[0] = -1;
	b127.v[1] = 2;
	b127.v[0] = 1;
	a127 += b127;
	EXPECT_EQ(a127.v[1], 4);
	EXPECT_EQ(a127.v[0], 0);

	vuint<127> c127;
	c127.v[1] = 99;
	c127.v[0] = 40;
	c127 -= 100;
	EXPECT_EQ(c127.v[1], 98);
	EXPECT_EQ(c127.v[0], -60);
}

TEST(TestVerilogUnsigned, Bit) {
	vuint<13> v13;
	vuint<127> v127;
	v13.v[0] = 0x123;
	v127.v[1] = 0xa;
	v127.v[0] = 0x5;
	EXPECT_TRUE(v13.Bit(0));
	EXPECT_FALSE(v13.Bit(2));
	EXPECT_FALSE(v13.Bit(12));
	EXPECT_TRUE(v127.Bit(2));
	EXPECT_FALSE(v127.Bit(3));
	EXPECT_FALSE(v127.Bit(64));
	EXPECT_TRUE(v127.Bit(65));
}

TEST(TestVerilogUnsigned, Shift) {
	vuint<10> v10;
	v10.v[0] = 0x3ff;
	v10 >>= 0;
	EXPECT_EQ(v10.v[0], 0x3ff);
	v10.v[0] = 0x3ff;
	v10 >>= 1;
	EXPECT_EQ(v10.v[0], 0x1ff);
	v10.v[0] = 0x3ff;
	v10 >>= 9;
	EXPECT_EQ(v10.v[0], 0x1);

	struct Pattern {
		string from_str, to_str;
		unsigned shamt;
	};
	Pattern patterns127[] {
		{"ffff_0f0f_0f0f_0f0f_0f0f", "ffff_0f0f_0f0f_0f0f_0f0f", 0},
		{"ffff_0f0f_0f0f_0f0f_0f0f", "7fff_0787_8787_8787_8787", 1},
		{"ffff_0f0f_0f0f_0f0f_0f0f", "fff_f0f0_f0f0_f0f0", 20},
		{"ffff_0f0f_0f0f_0f0f_0f0f", "1fffe", 63},
		{"ffff_0f0f_0f0f_0f0f_0f0f", "ffff", 64},
		{"ffff_0f0f_0f0f_0f0f_0f0f", "7fff", 65}
	};
	for (auto &p: patterns127) {
		vuint<127> from_v127, to_v127;
		from_hex(from_v127, p.from_str);
		from_v127 >>= p.shamt;
		from_hex(to_v127, p.to_str);
		EXPECT_EQ(p.from_str, p.to_str);
	}
}

TEST(TestVerilogUnsigned, Unary) {
}
