// Direct include
// C system headers
// C++ standard library headers
#include <array>
#include <functional>
#include <string>
#include <tuple>
#include <type_traits>
// Other libraries' .h files.
#include <gtest/gtest.h>
// Your project's .h files.
#include "verilog/dtype/vint2.h"
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

TEST(TestVerilogUnsigned, Constructor) {
	{
		vuint2<7> v7(0x3fu);
		EXPECT_EQ(v7.v, uint8_t(0x3fu));
	}
	{
		vuint2<7> v7(0x7fu);
		EXPECT_EQ(v7.v, uint8_t(0x7fu));
	}
	{
		vuint2<7> v7(0xffu);
		EXPECT_EQ(v7.v, uint8_t(0x7fu));
	}
	{
		vuint2<7> v7(-1);
		EXPECT_EQ(v7.v, uint8_t(0x7fu));
	}
	{
		vuint2<8> v8(0x7fu);
		EXPECT_EQ(v8.v, uint8_t(0x7fu));
	}
	{
		vuint2<8> v8(0xffu);
		EXPECT_EQ(v8.v, uint8_t(0xffu));
	}
	{
		vuint2<8> v8(-1);
		EXPECT_EQ(v8.v, uint8_t(0xffu));
	}
	{
		vuint2<10> v10(0xf00u);
		EXPECT_EQ(v10.v, uint16_t(0x300u));
	}
	{
		vuint2<31> v31(0xf1230000u);
		EXPECT_EQ(v31.v, uint32_t(0x71230000u));
	}
	{
		vuint2<32> v32(0xf1230000u);
		EXPECT_EQ(v32.v, uint32_t(0xf1230000u));
	}
	{
		vuint2<33> v33(0xff1230000u);
		EXPECT_EQ(v33.v, uint64_t(0x1f1230000u));
	}
	{
		vuint2<127> v127({
			0x0123456701234567llu,
			0x3fffffffffffffffllu
		});
		EXPECT_EQ(v127.v[0], uint64_t(0x0123456701234567llu));
		EXPECT_EQ(v127.v[1], uint64_t(0x3fffffffffffffffllu));
	}
	{
		vuint2<127> v127({
			0x0123456701234567llu,
			0xffffffffffffffffllu
		});
		EXPECT_EQ(v127.v[0], uint64_t(0x0123456701234567llu));
		EXPECT_EQ(v127.v[1], uint64_t(0x7fffffffffffffffllu));
	}
	{
		vuint2<128> v128({
			0x0123456701234567llu,
			0xffffffffffffffffllu
		});
		EXPECT_EQ(v128.v[0], uint64_t(0x0123456701234567llu));
		EXPECT_EQ(v128.v[1], uint64_t(0xffffffffffffffffllu));
	}
}
