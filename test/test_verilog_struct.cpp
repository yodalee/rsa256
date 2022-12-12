#include "verilog_int.h"
#include "verilog_struct.h"
#include <gtest/gtest.h>
#include <array>
#include <cstring>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
using namespace std;
using namespace verilog;

typedef vuint<13> Type13;

struct Struct01 {
	BOOST_HANA_DEFINE_STRUCT(Struct01,
		(vuint<8>, v8)
	);
	VERILOG_DEFINE_STRUCT_EXTRA(Struct01)
};

struct Struct02 {
	BOOST_HANA_DEFINE_STRUCT(Struct02,
		(vuint<8>, v8),
		(vuint<99>, v99),
		(vuint<17>, v17)
	);
	VERILOG_DEFINE_STRUCT_EXTRA(Struct02)
};

struct Struct03 {
	BOOST_HANA_DEFINE_STRUCT(Struct03,
		(Type13, v13),
		(Struct01, s1),
		(Struct02, s2)
	);
	VERILOG_DEFINE_STRUCT_EXTRA(Struct03)
};

TEST(TestComputeStructBit, StructSimple) {
	constexpr unsigned a = num_bit_of<Type13>;
	constexpr unsigned b = num_bit_of<Struct01>;
	constexpr unsigned c = num_bit_of<Struct02>;
	constexpr unsigned d = num_bit_of<Struct03>;
	static_assert(is_trivially_copyable_v<Type13>);
	static_assert(is_trivially_copyable_v<Struct01>);
	static_assert(is_trivially_copyable_v<Struct02>);
	static_assert(is_trivially_copyable_v<Struct03>);
	static_assert(a == 13);
	static_assert(b == 8);
	static_assert(c == 8+99+17);
	static_assert(a+b+c == d);
}

TEST(TestPrintStruct, StructSimple) {
	Struct03 s;
	stringstream ss;
	s.v13 = 1;
	s.s1.v8 = 2;
	s.s2.v8 = 3;
	s.s2.v99 = 4;
	s.s2.v17 = 5;
	PrintContent(ss, s);
	EXPECT_EQ(ss.str(), "{v13:1,s1:{v8:2,},s2:{v8:3,v99:4,v17:5,},},");
}

TEST(TestTypeString, StructSimple) {
	const string a = GetTypeString(Type13{});
	const string b = GetTypeString(Struct01{});
	const string c = GetTypeString(Struct02{});
	const string d = GetTypeString(Struct03{});
	EXPECT_EQ(a, "u13");
	EXPECT_EQ(b, "u8");
	EXPECT_EQ(c, "u8u99u17");
	EXPECT_EQ(d, a+b+c);
}

TEST(TestLoadSave, StructSimple) {
	stringstream ss;
	constexpr int N = 10;
	array<Struct03, N> from_data;
	for (int i = 0, j = 1; i < N; ++i) {
		from_data[i].v13 = j++;
		from_data[i].s1.v8 = j++;
		from_data[i].s2.v8 = j++;
		from_data[i].s2.v99 = j++;
		from_data[i].s2.v17 = j++;
	}

	// Store
	const string ts_s03 = GetTypeString(Struct03{});
	ss << ts_s03 << '$';
	for (int i = 0; i < N; ++i) {
		SaveContent(ss, from_data[i]);
		EXPECT_TRUE(ss.good());
	}

	// Load + check contents
	string ts_s03_gotten;
	getline(ss, ts_s03_gotten, '$');
	EXPECT_EQ(ts_s03, ts_s03_gotten);
	for (int i = 0; i < N; ++i) {
		Struct03 to_datum;
		// zero-initialize
		memset(reinterpret_cast<char*>(&to_datum), 0, sizeof(Struct03));
		// shall not equal to all zero now
		EXPECT_NE(to_datum, from_data[i]);
		// load
		LoadContent(ss, to_datum);
		// and then check
		EXPECT_EQ(to_datum, from_data[i]);
		EXPECT_TRUE(ss.good());
	}
	// we shall be at the end of stringstream
	char c;
	EXPECT_FALSE(ss.get(c));
	EXPECT_TRUE(ss.eof());
}
