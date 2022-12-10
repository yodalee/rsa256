#include "verilog_int.h"
#include "verilog_struct.h"
#include <gtest/gtest.h>
#include <string>
#include <tuple>
#include <array>
using namespace std;
using namespace verilog;

typedef vuint<13> Type13;

struct Struct01 {
	BOOST_HANA_DEFINE_STRUCT(Struct01,
		(vuint<8>, v8)
	);
};

struct Struct02 {
	BOOST_HANA_DEFINE_STRUCT(Struct02,
		(vuint<8>, v8),
		(vuint<99>, v99),
		(vuint<17>, v17)
	);
};

struct Struct03 {
	BOOST_HANA_DEFINE_STRUCT(Struct03,
		(Type13, v13),
		(Struct01, s1),
		(Struct02, s2)
	);
};

TEST(TestVerilogStruct, StructSimple) {
	constexpr unsigned a = num_bit_of<Type13>;
	constexpr unsigned b = num_bit_of<Struct01>;
	constexpr unsigned c = num_bit_of<Struct02>;
	constexpr unsigned d = num_bit_of<Struct03>;
	static_assert(a == 13);
	static_assert(b == 8);
	static_assert(c == 8+99+17);
	static_assert(a+b+c == d);
}
