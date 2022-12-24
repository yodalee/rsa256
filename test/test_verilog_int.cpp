#include "verilog_int.h"
#include <gtest/gtest.h>
#include <array>
#include <functional>
#include <string>
#include <tuple>
#include <type_traits>
using namespace std;
using namespace verilog;

///////////////////////////
// InternalLayout tests the internal storage directly
// Normally user don't need to access the data in this way
///////////////////////////
template<template<unsigned> class IntTmpl>
void InternalLayoutTemplate() {
	IntTmpl<7> v7;
	v7.v[0] = 0x3fu;
	v7.ClearUnusedBits();
	EXPECT_EQ(v7.v[0], uint8_t(0x3fu));
	v7.v[0] = 0x7fu;
	v7.ClearUnusedBits();
	EXPECT_EQ(v7.v[0], uint8_t(0x7fu));
	v7.v[0] = -1;
	v7.ClearUnusedBits();
	EXPECT_EQ(v7.v[0], uint8_t(0x7fu));
	v7.v[0] = 0xffu;
	v7.ClearUnusedBits();
	EXPECT_EQ(v7.v[0], uint8_t(0x7fu));

	IntTmpl<8> v8;
	v8.v[0] = 0x7fu;
	v8.ClearUnusedBits();
	EXPECT_EQ(v8.v[0], uint8_t(0x7fu));
	v8.v[0] = -1;
	v8.ClearUnusedBits();
	EXPECT_EQ(v8.v[0], uint8_t(0xffu));
	v8.v[0] = 0xffu;
	v8.ClearUnusedBits();
	EXPECT_EQ(v8.v[0], uint8_t(0xffu));

	IntTmpl<10> v10;
	v10.v[0] = 0xf00u;
	v10.ClearUnusedBits();
	EXPECT_EQ(v10.v[0], uint16_t(0x300u));

	IntTmpl<31> v31;
	v31.v[0] = 0xf0000000u;
	v31.ClearUnusedBits();
	EXPECT_EQ(v31.v[0], uint32_t(0x70000000u));

	IntTmpl<33> v33;
	v33.v[0] = 0xf00000000llu;
	v33.ClearUnusedBits();
	EXPECT_EQ(v33.v[0], uint64_t(0x100000000llu));

	IntTmpl<127> v127;
	v127.v[0] = 0x0123456701234567llu;
	v127.v[1] = 0x3fffffffffffffffllu;
	v127.ClearUnusedBits();
	EXPECT_EQ(v127.v[0], uint64_t(0x0123456701234567llu));
	EXPECT_EQ(v127.v[1], uint64_t(0x3fffffffffffffffllu));
	v127.v[0] = 0x0123456701234567llu;
	v127.v[1] = 0xffffffffffffffffllu;
	v127.ClearUnusedBits();
	EXPECT_EQ(v127.v[0], uint64_t(0x0123456701234567llu));
	EXPECT_EQ(v127.v[1], uint64_t(0x7fffffffffffffffllu));

	IntTmpl<128> v128;
	v128.v[0] = 0x0123456701234567llu;
	v128.v[1] = 0xffffffffffffffffllu;
	v128.ClearUnusedBits();
	EXPECT_EQ(v128.v[0], uint64_t(0x0123456701234567llu));
	EXPECT_EQ(v128.v[1], uint64_t(0xffffffffffffffffllu));

	static_assert(is_same_v<typename decltype(v7)::stype, uint8_t>);
	static_assert(is_same_v<typename decltype(v8)::stype, uint8_t>);
	static_assert(is_same_v<typename decltype(v10)::stype, uint16_t>);
	static_assert(is_same_v<typename decltype(v31)::stype, uint32_t>);
	static_assert(is_same_v<typename decltype(v33)::stype, uint64_t>);
	static_assert(is_same_v<typename decltype(v127)::stype, uint64_t>);
	static_assert(is_same_v<typename decltype(v128)::stype, uint64_t>);
}

TEST(TestVerilogUnsigned, InternalLayout) {
	InternalLayoutTemplate<vuint>();
}

TEST(TestVerilogSigned, InternalLayout) {
	InternalLayoutTemplate<vsint>();
}

///////////////////////////
// Test rule of three
// Mainly test on assigning C++ native type
///////////////////////////
template<template<unsigned> class IntTmpl>
void RuleOfThreeTemplate() {
	IntTmpl<7> v7(0x2a);
	EXPECT_EQ(v7.v[0], uint8_t(0x2au));
	v7 = 0xff;
	EXPECT_EQ(v7.v[0], uint8_t(0x7fu));
	v7 = -1;
	EXPECT_EQ(v7.v[0], uint8_t(0x7fu));

	IntTmpl<13> v13(0xffff);
	EXPECT_EQ(v13.v[0], uint16_t(0x1fff));
	v13 = -1;
	EXPECT_EQ(v13.v[0], uint16_t(0x1fff));

	// The sign extension rule of >64b signals are described here
	{
		// No sign extension
		IntTmpl<99> v99(5566, false);
		EXPECT_EQ(v99.v[0], uint64_t(5566));
		EXPECT_EQ(v99.v[1], uint64_t(0));
	}
	{
		// No sign extension
		IntTmpl<99> v99(1234, true);
		EXPECT_EQ(v99.v[0], uint64_t(1234));
		EXPECT_EQ(v99.v[1], uint64_t(0));
	}
	{
		// Forcely no sign extension
		IntTmpl<99> v99(-123, false);
		EXPECT_EQ(v99.v[0], uint64_t(-123));
		EXPECT_EQ(v99.v[1], uint64_t(0));
	}
	{
		IntTmpl<99> v99(0);
		// Force no sign extenstion
		v99.uassign(-2);
		EXPECT_EQ(v99.v[0], uint64_t(-2));
		EXPECT_EQ(v99.v[1], uint64_t(0));
		// Force sign extenstion
		v99.sassign(-3);
		EXPECT_EQ(v99.v[0], uint64_t(-3));
		EXPECT_EQ(v99.v[1], uint64_t(0x7ffffffffllu));
		// No sign extension
		v99.uassign(4);
		EXPECT_EQ(v99.v[0], uint64_t(4));
		EXPECT_EQ(v99.v[1], uint64_t(0));
		// No sign extension
		v99.sassign(5);
		EXPECT_EQ(v99.v[0], uint64_t(5));
		EXPECT_EQ(v99.v[1], uint64_t(0));
	}
}

TEST(TestVerilogUnsigned, RuleOfThree) {
	RuleOfThreeTemplate<vuint>();

	// The sign extension rule of >64b signals are described here
	// No sign extension by default
	vuint<99> v99(-1000);
	EXPECT_EQ(v99.v[0], uint64_t(-1000));
	EXPECT_EQ(v99.v[1], uint64_t(0));
	// No sign extension by default
	v99 = -1;
	EXPECT_EQ(v99.v[0], uint64_t(-1));
	EXPECT_EQ(v99.v[1], uint64_t(0));
}

TEST(TestVerilogSigned, RuleOfThree) {
	RuleOfThreeTemplate<vsint>();

	// The sign extension rule of >64b signals are described here
	// Sign extension by default
	vsint<99> v99(-1000);
	EXPECT_EQ(v99.v[0], uint64_t(-1000));
	EXPECT_EQ(v99.v[1], uint64_t(0x7ffffffffllu));
	// Sign extension by default
	v99 = -1;
	EXPECT_EQ(v99.v[0], uint64_t(-1));
	EXPECT_EQ(v99.v[1], uint64_t(0x7ffffffffllu));
}

///////////////////////////
// Test from hex conversion
///////////////////////////
template<template<unsigned> class IntTmpl>
void FromHexTemplate() {
	IntTmpl<8> v8;
	from_hex(v8, "2A");
	EXPECT_EQ(v8.v[0], 0x2A);
	// sign extension
	from_hex(v8, "'2A");
	EXPECT_EQ(v8.v[0], 0xEA);
	// negate
	from_hex(v8, "-1");
	EXPECT_EQ(v8.v[0], 0xff);
	// Note: mixing sign extension and negate is un-specified

	IntTmpl<13> v13;
	// zero
	from_hex(v13, "");
	EXPECT_EQ(v13.v[0], 0);
	from_hex(v13, "1aC");
	EXPECT_EQ(v13.v[0], 0x1ac);
	from_hex(v13, "5");
	EXPECT_EQ(v13.v[0], 0x5);
	// no sign extension
	from_hex(v13, "'05");
	EXPECT_EQ(v13.v[0], 0x5);
	// sign extension
	from_hex(v13, "'5");
	EXPECT_EQ(v13.v[0], 0x1ffd);
	from_hex(v13, "-1");
	EXPECT_EQ(v13.v[0], 0x1fff);
	// non-recognized character are ignored
	from_hex(v13, "1..2..3..4");
	EXPECT_EQ(v13.v[0], 0x1234);
	from_hex(v13, "ffff");
	EXPECT_EQ(v13.v[0], 0x1fff);
	from_hex(v13, "00000f");
	EXPECT_EQ(v13.v[0], 0xf);

	IntTmpl<127> v127;
	from_hex(v127, "8123_4567_acac_acac_89AB_cdef_0000_5555");
	EXPECT_EQ(v127.v[0], 0x89ABCDEF00005555llu);
	EXPECT_EQ(v127.v[1], 0x01234567acacacacllu);
	from_hex(v127, "00000000 8123 4567 acac acac 89AB cdef 0000 5555");
	EXPECT_EQ(v127.v[0], 0x89ABCDEF00005555llu);
	EXPECT_EQ(v127.v[1], 0x01234567acacacacllu);
	from_hex(v127, "3_4567_acac_acac_89AB_cdef_0000_5555");
	EXPECT_EQ(v127.v[0], 0x89ABCDEF00005555llu);
	EXPECT_EQ(v127.v[1], 0x00034567acacacacllu);
	from_hex(v127, "'3_4567_acac_acac_89AB_cdef_0000_5555");
	EXPECT_EQ(v127.v[0], 0x89ABCDEF00005555llu);
	EXPECT_EQ(v127.v[1], 0x7fff4567acacacacllu);
	from_hex(v127, "'03_4567_acac_acac_89AB_cdef_0000_5555");
	EXPECT_EQ(v127.v[0], 0x89ABCDEF00005555llu);
	EXPECT_EQ(v127.v[1], 0x00034567acacacacllu);
	from_hex(v127, "'5");
	EXPECT_EQ(v127.v[0], 0xfffffffffffffffdllu);
	EXPECT_EQ(v127.v[1], 0x7fffffffffffffffllu);
	from_hex(v127, "5");
	EXPECT_EQ(v127.v[0], 0x5llu);
	EXPECT_EQ(v127.v[1], 0llu);
	from_hex(v127, "");
	EXPECT_EQ(v127.v[0], 0llu);
	EXPECT_EQ(v127.v[1], 0llu);
}

TEST(TestVerilogUnsigned, FromHex) {
	FromHexTemplate<vuint>();
}

TEST(TestVerilogSigned, FromHex) {
	FromHexTemplate<vsint>();
}

///////////////////////////
// Test to hex conversion
///////////////////////////
template<template<unsigned> class IntTmpl>
void ToHexTemplate() {
	IntTmpl<8> v8;
	v8.v[0] = 0x2a;
	v8.ClearUnusedBits();
	EXPECT_EQ(to_hex(v8), "2A");

	v8.v[0] = 0;
	EXPECT_EQ(to_hex(v8), "0");

	IntTmpl<13> v13;
	v13.v[0] = 0x1a2a;
	v8.ClearUnusedBits();
	EXPECT_EQ(to_hex(v13), "1A2A");

	IntTmpl<66> v66;
	v66.v[1] = 0x2;
	v66.v[0] = 0x0123456789abcdefllu;
	v66.ClearUnusedBits();
	EXPECT_EQ(to_hex(v66), "20123456789ABCDEF");

	IntTmpl<90> v90;
	v90.v[1] = 0xfllu;
	v90.v[0] = 0x5566556612345678llu;
	v90.ClearUnusedBits();
	EXPECT_EQ(to_hex(v90), "F5566556612345678");
}

TEST(TestVerilogUnsigned, ToHex) {
	ToHexTemplate<vuint>();
}

TEST(TestVerilogSigned, DISABLED_ToHex) {
	ToHexTemplate<vsint>();
}

///////////////////////////
// Test comparison (EQ/NE)
///////////////////////////
template<template<unsigned> class IntTmpl>
void CompareEQTemplate() {
	IntTmpl<7> a7, b7;
	IntTmpl<99> a99, b99;

	a7 = -1;
	b7 = -1;
	EXPECT_EQ(a7, b7);
	EXPECT_EQ(a7, -1);
	EXPECT_EQ(b7, -1);
	EXPECT_EQ(a7, 127);
	EXPECT_EQ(b7, 127);
	EXPECT_NE(a7, 255);
	EXPECT_NE(b7, 255);
	a7 = 255;
	b7 = -1;
	EXPECT_EQ(a7, b7);
	a7 = 128 + 3;
	b7 = 0 + 3;
	EXPECT_EQ(a7, b7);
	a7 = 1;
	b7 = 0;
	EXPECT_NE(a7, b7);
	a7 = 0xf;
	b7 = -1;
	EXPECT_NE(a7, b7);

	a99 = 123;
	b99 = 123;
	EXPECT_EQ(a99, b99);
	a99 = -123;
	b99 = -123;
	EXPECT_EQ(a99, b99);
	a99 = -1;
	b99 = 123;
	EXPECT_NE(a99, b99);
}

TEST(TestVerilogUnsigned, CompareEQ) {
	CompareEQTemplate<vuint>();
}

TEST(TestVerilogSigned, CompareEQ) {
	CompareEQTemplate<vsint>();
}

///////////////////////////
// Test bit extraction
///////////////////////////
template<template<unsigned> class IntTmpl>
void BitExtractionTemplate() {
	IntTmpl<13> v13;
	IntTmpl<127> v127;
	v13.v[0] = 0x123;
	v127.v[1] = 0xa;
	v127.v[0] = 0x5;
	v13.ClearUnusedBits();
	v127.ClearUnusedBits();
	EXPECT_TRUE(v13.Bit(0));
	EXPECT_FALSE(v13.Bit(2));
	EXPECT_FALSE(v13.Bit(12));
	EXPECT_TRUE(v127.Bit(2));
	EXPECT_FALSE(v127.Bit(3));
	EXPECT_FALSE(v127.Bit(64));
	EXPECT_TRUE(v127.Bit(65));
}

TEST(TestVerilogUnsigned, BitExtraction) {
	BitExtractionTemplate<vuint>();
}

TEST(TestVerilogSigned, BitExtraction) {
	BitExtractionTemplate<vsint>();
}

///////////////////////////
// Test +-*/
///////////////////////////
#if 0
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

TEST(TestVerilogSigned, DISABLED_AddSubMulDiv) {
}
#endif

///////////////////////////
// Test shift
///////////////////////////
static const tuple<array<uint16_t, 2>, unsigned> patterns_ru10[] {
	{{0x1a5, 0x01a}, 4u},
	{{0x3a5, 0x3a5}, 0u},
	{{0x3a5, 0x1d2}, 1u},
	{{0x3a5, 0x03a}, 4u},
	{{0x3a5, 0x001}, 9u}
};
static const tuple<array<uint16_t, 2>, unsigned> patterns_rs10[] {
	{{0x1a5, 0x01a}, 4u},
	{{0x3a5, 0x3a5}, 0u},
	{{0x3a5, 0x3d2}, 1u},
	{{0x3a5, 0x3fa}, 4u},
	{{0x3a5, 0x3ff}, 9u}
};
static const tuple<array<uint16_t, 2>, unsigned> patterns_l10[] {
	{{0x3a5, 0x3a5}, 0u},
	{{0x3a5, 0x250}, 4u},
	{{0x3a5, 0x100}, 8u}
};
static const tuple<array<uint64_t, 6>, unsigned> patterns_ru136[] {
	{{
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu,
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu,
	}, 0u },
	{{
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu,
		0x0000'0000'0000'000fllu, 0xfabc'def0'1234'5678llu, 0x90f0'f0f0'f0f0'f0f0llu,
	}, 4u },
	{{
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu,
		0x0000'0000'0000'0000llu, 0xffab'cdef'0123'4567llu, 0x890f'0f0f'0f0f'0f0fllu,
	}, 8u },
	{{
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu,
		0x0000'0000'0000'0000llu, 0x0ffa'bcde'f012'3456llu, 0x7890'f0f0'f0f0'f0f0llu,
	}, 12u },
	{{
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu,
		0x0000'0000'0000'0000llu, 0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu,
	}, 64u },
	{{
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu,
		0x0000'0000'0000'0000llu, 0x0000'0000'0000'000fllu, 0xfabc'def0'1234'5678llu,
	}, 68u },
	{{
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu,
		0x0000'0000'0000'0000llu, 0x0000'0000'0000'0000llu, 0xffab'cdef'0123'4567llu,
	}, 72u },
	{{
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu,
		0x0000'0000'0000'0000llu, 0x0000'0000'0000'0000llu, 0x0000'0000'0000'00ffllu,
	}, 128u },
	{{
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu,
		0x0000'0000'0000'0000llu, 0x0000'0000'0000'0000llu, 0x0000'0000'0000'000fllu,
	}, 132u }
};
static const tuple<array<uint64_t, 6>, unsigned> patterns_rs136[] {
	{{
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu,
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu,
	}, 0u },
	{{
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu,
		0x0000'0000'0000'00ffllu, 0xfabc'def0'1234'5678llu, 0x90f0'f0f0'f0f0'f0f0llu,
	}, 4u },
	{{
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu,
		0x0000'0000'0000'00ffllu, 0xffab'cdef'0123'4567llu, 0x890f'0f0f'0f0f'0f0fllu,
	}, 8u },
	{{
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu,
		0x0000'0000'0000'00ffllu, 0xfffa'bcde'f012'3456llu, 0x7890'f0f0'f0f0'f0f0llu,
	}, 12u },
	{{
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu,
		0x0000'0000'0000'00ffllu, 0xffff'ffff'ffff'ffffllu, 0xabcd'ef01'2345'6789llu,
	}, 64u },
	{{
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu,
		0x0000'0000'0000'00ffllu, 0xffff'ffff'ffff'ffffllu, 0xfabc'def0'1234'5678llu,
	}, 68u },
	{{
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu,
		0x0000'0000'0000'00ffllu, 0xffff'ffff'ffff'ffffllu, 0xffab'cdef'0123'4567llu,
	}, 72u },
	{{
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu,
		0x0000'0000'0000'00ffllu, 0xffff'ffff'ffff'ffffllu, 0xffff'ffff'ffff'ffffllu,
	}, 128u },
	{{
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu,
		0x0000'0000'0000'00ffllu, 0xffff'ffff'ffff'ffffllu, 0xffff'ffff'ffff'ffffllu,
	}, 132u }
};
static const tuple<array<uint64_t, 6>, unsigned> patterns_l136[] {
	{{
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu,
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu
	}, 0u },
	{{
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu,
		0x0000'0000'0000'0001llu, 0x2345'6789'0f0f'0f0fllu, 0x0f0f'0f0f'0000'0000llu
	}, 32u },
	{{
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu,
		0x0000'0000'0000'0089llu, 0x0f0f'0f0f'0f0f'0f0fllu, 0x0000'0000'0000'0000llu
	}, 64u },
	{{
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu,
		0x0000'0000'0000'000fllu, 0x0f0f'0f0f'0000'0000llu, 0x0000'0000'0000'0000llu
	}, 96u},
	{{
		0x0000'0000'0000'00ffllu, 0xabcd'ef01'2345'6789llu, 0x0f0f'0f0f'0f0f'0f0fllu,
		0x0000'0000'0000'000fllu, 0x0000'0000'0000'0000llu, 0x0000'0000'0000'0000llu,
	}, 128u}
};

template<template<unsigned> class IntTmpl>
void ShiftTemplate() {
	IntTmpl<10> v10;
	for (auto &p: patterns_ru10) {
		const auto &test_data = get<0>(p);
		const unsigned nshift = get<1>(p);
		v10.v[0] = test_data[0];
		// force unsigned
		v10.ushiftopr(nshift);
		EXPECT_EQ(v10.v[0], test_data[1]) << "rshift " << nshift;
	}
	for (auto &p: patterns_rs10) {
		const auto &test_data = get<0>(p);
		const unsigned nshift = get<1>(p);
		v10.v[0] = test_data[0];
		// force signed
		v10.sshiftopr(nshift);
		EXPECT_EQ(v10.v[0], test_data[1]) << "rshift " << nshift;
	}
	for (auto &p: patterns_l10) {
		const auto &test_data = get<0>(p);
		const unsigned nshift = get<1>(p);
		v10.v[0] = test_data[0];
		v10 <<= nshift;
		EXPECT_EQ(v10.v[0], test_data[1]) << "lshift " << nshift;
	}

	IntTmpl<136> v136;
	for (auto &p: patterns_ru136) {
		const auto &test_data = get<0>(p);
		const unsigned nshift = get<1>(p);
		v136.v[0] = test_data[2];
		v136.v[1] = test_data[1];
		v136.v[2] = test_data[0];
		// force unsigned
		v136.ushiftopr(nshift);
		EXPECT_EQ(v136.v[0], test_data[5]) << "rshift " << nshift;
		EXPECT_EQ(v136.v[1], test_data[4]) << "rshift " << nshift;
		EXPECT_EQ(v136.v[2], test_data[3]) << "rshift " << nshift;
	}
	for (auto &p: patterns_rs136) {
		const auto &test_data = get<0>(p);
		const unsigned nshift = get<1>(p);
		v136.v[0] = test_data[2];
		v136.v[1] = test_data[1];
		v136.v[2] = test_data[0];
		// force signed
		v136.sshiftopr(nshift);
		EXPECT_EQ(v136.v[0], test_data[5]) << "lshift " << nshift;
		EXPECT_EQ(v136.v[1], test_data[4]) << "lshift " << nshift;
		EXPECT_EQ(v136.v[2], test_data[3]) << "lshift " << nshift;
	}
	for (auto &p: patterns_l136) {
		const auto &test_data = get<0>(p);
		const unsigned nshift = get<1>(p);
		v136.v[0] = test_data[2];
		v136.v[1] = test_data[1];
		v136.v[2] = test_data[0];
		v136 <<= nshift;
		EXPECT_EQ(v136.v[0], test_data[5]) << "lshift " << nshift;
		EXPECT_EQ(v136.v[1], test_data[4]) << "lshift " << nshift;
		EXPECT_EQ(v136.v[2], test_data[3]) << "lshift " << nshift;
	}
}

TEST(TestVerilogUnsigned, Shift) {
	ShiftTemplate<vuint>();

	vuint<10> v10;
	for (auto &p: patterns_ru10) {
		const auto &test_data = get<0>(p);
		const unsigned nshift = get<1>(p);
		v10.v[0] = test_data[0];
		// default unsigned
		v10 >>= nshift;
		EXPECT_EQ(v10.v[0], test_data[1]);
	}

	vuint<136> v136;
	for (auto &p: patterns_ru136) {
		const auto &test_data = get<0>(p);
		const unsigned nshift = get<1>(p);
		v136.v[0] = test_data[2];
		v136.v[1] = test_data[1];
		v136.v[2] = test_data[0];
		// force unsigned
		v136.ushiftopr(nshift);
		EXPECT_EQ(v136.v[0], test_data[5]) << "rshift " << nshift;
		EXPECT_EQ(v136.v[1], test_data[4]) << "rshift " << nshift;
		EXPECT_EQ(v136.v[2], test_data[3]) << "rshift " << nshift;
	}
}

TEST(TestVerilogSigned, Shift) {
	ShiftTemplate<vsint>();

	vsint<10> v10;
	for (auto &p: patterns_rs10) {
		const auto &test_data = get<0>(p);
		const unsigned nshift = get<1>(p);
		v10.v[0] = test_data[0];
		// default signed
		v10 >>= nshift;
		EXPECT_EQ(v10.v[0], test_data[1]);
	}

	vuint<136> v136;
	for (auto &p: patterns_rs136) {
		const auto &test_data = get<0>(p);
		const unsigned nshift = get<1>(p);
		v136.v[0] = test_data[2];
		v136.v[1] = test_data[1];
		v136.v[2] = test_data[0];
		// default signed
		v136.sshiftopr(nshift);
		EXPECT_EQ(v136.v[0], test_data[5]) << "rshift " << nshift;
		EXPECT_EQ(v136.v[1], test_data[4]) << "rshift " << nshift;
		EXPECT_EQ(v136.v[2], test_data[3]) << "rshift " << nshift;
	}
}

///////////////////////////
// Test negate
///////////////////////////
static const array<uint8_t, 2> pattern_neg7[]{
	{0x2a, 0x56},
	{0x7f, 0x01},
	{0x00, 0x00}
};
static const array<uint64_t, 4> pattern_neg127[]{
	{
		0x0000'0000'0000'0000ll, 0x0000'0000'0000'0001ll,
		0x7fff'ffff'ffff'ffffll, 0xffff'ffff'ffff'ffffll
	},
	{
		0x7fff'ffff'ffff'0000ll, 0x0000'0000'0000'0001ll,
		0x0000'0000'0000'ffffll, 0xffff'ffff'ffff'ffffll
	},
	{
		0x0000'0000'0000'0000ll, 0x0000'0000'0000'0000ll,
		0x0000'0000'0000'0000ll, 0x0000'0000'0000'0000ll
	}
};

template<template<unsigned> class IntTmpl>
void NegateTemplate() {
	IntTmpl<7> v7;
	for (auto &p: pattern_neg7) {
		v7.v[0] = p[0];
		v7.Negate();
		EXPECT_EQ(v7.v[0], p[1]);
		// do it reversely
		v7.Negate();
		EXPECT_EQ(v7.v[0], p[0]);
	}

	IntTmpl<127> v127;
	for (auto &p: pattern_neg127) {
		v127.v[0] = p[1];
		v127.v[1] = p[0];
		v127.Negate();
		EXPECT_EQ(v127.v[0], p[3]);
		EXPECT_EQ(v127.v[1], p[2]);
		// do it reversely
		v127.Negate();
		EXPECT_EQ(v127.v[0], p[1]);
		EXPECT_EQ(v127.v[1], p[0]);
	}
}

TEST(TestVerilogUnsigned, Negate) {
	NegateTemplate<vuint>();
}

TEST(TestVerilogSigned, Negate) {
	NegateTemplate<vsint>();
}

///////////////////////////
// Test flip
///////////////////////////
static const array<uint8_t, 2> pattern_flip7[]{
	{0x2a, 0x55},
	{0x7f, 0x00}
};
static const array<uint64_t, 4> pattern_flip127[]{
	{
		0x0000'0000'0000'0000ll, 0x0000'0000'0000'0000ll,
		0x7fff'ffff'ffff'ffffll, 0xffff'ffff'ffff'ffffll
	},
	{
		0x7fff'ffff'ffff'0000ll, 0x0000'0000'0000'0000ll,
		0x0000'0000'0000'ffffll, 0xffff'ffff'ffff'ffffll
	}
};

template<template<unsigned> class IntTmpl>
void FlipTemplate() {
	IntTmpl<7> v7;
	for (auto &p: pattern_flip7) {
		v7.v[0] = p[0];
		v7.Flip();
		EXPECT_EQ(v7.v[0], p[1]);
		// do it reversely
		v7.Flip();
		EXPECT_EQ(v7.v[0], p[0]);
	}

	IntTmpl<127> v127;
	for (auto &p: pattern_flip127) {
		v127.v[0] = p[1];
		v127.v[1] = p[0];
		v127.Flip();
		EXPECT_EQ(v127.v[0], p[3]);
		EXPECT_EQ(v127.v[1], p[2]);
		// do it reversely
		v127.Flip();
		EXPECT_EQ(v127.v[0], p[1]);
		EXPECT_EQ(v127.v[1], p[0]);
	}
}

TEST(TestVerilogUnsigned, Flip) {
	FlipTemplate<vuint>();
}

TEST(TestVerilogSigned, Flip) {
	FlipTemplate<vsint>();
}

#if 0
///////////////////////////
// Test cast
///////////////////////////
TEST(TestVerilogUnsigned, ExplicitCast) {
	{
		vuint<5> v5;
		vuint<100> v100;
		v5 = 12;
		v100 = vuint<100>{v5};
		EXPECT_EQ(v100.v[0], 12);
		EXPECT_EQ(v100.v[1], 0);
	}

	{
		vuint<5> v5;
		vuint<100> v100;
		v100.v[0] = 0xff;
		v100.v[1] = 0x99;
		v5 = vuint<5>{v100};
		EXPECT_EQ(v5, 0x1f);
	}

	{
		vuint<67> v67;
		vuint<130> v130;
		v67.v[0] = -1;
		v67.v[1] = 0x7;
		v130 = vuint<130>{v67};
		EXPECT_EQ(v130.v[0], uint64_t(-1));
		EXPECT_EQ(v130.v[1], 0x7);
		EXPECT_EQ(v130.v[2], 0);
	}

	{
		vuint<67> v67;
		vuint<130> v130;
		v130.v[0] = 5566;
		v130.v[1] = -1;
		v130.v[2] = 1;
		v67 = vuint<67>{v130};
		EXPECT_EQ(v67.v[0], 5566);
		EXPECT_EQ(v67.v[1], 0x7);
	}
}

TEST(TestVerilogSigned, ExplicitCast) {
}

#endif
