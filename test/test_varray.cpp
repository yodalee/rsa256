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
#include "verilog/dtype/vint.h"
#include "verilog/dtype/varray.h"
using namespace std;
using namespace verilog;
typedef varray<vuint<10>, 5> Arr1;
typedef varray<vuint<3>, 2, 3> Arr2;

TEST(TestVerilogArray, Basic) {
	Arr1 v10x5;
	Arr2 v3x2x3;
	static_assert(bits<Arr1>() == 50);
	static_assert(bits<Arr2>() == 18);
	static_assert(Arr1::asize == 5);
	static_assert(Arr2::asize == 6);
	static_assert(is_varray_v<Arr1>);
	static_assert(is_varray_v<Arr2>);

	{
		auto it = v10x5.begin();
		auto it_end = v10x5.end();
		for (unsigned i = 0; i < Arr1::asize; ++i, ++it) {
			*it = i;
		}
		EXPECT_EQ(v10x5[0], 0);
		EXPECT_EQ(v10x5[1], 1);
		EXPECT_EQ(v10x5[2], 2);
		EXPECT_EQ(v10x5[3], 3);
		EXPECT_EQ(v10x5[4], 4);
		EXPECT_EQ(it, it_end);
	}
	{
		auto it = v3x2x3.begin();
		auto it_end = v3x2x3.end();
		for (unsigned i = 0; i < Arr2::asize; ++i, ++it) {
			*it = i;
		}
		EXPECT_EQ(v3x2x3[0][0], 0);
		EXPECT_EQ(v3x2x3[0][1], 1);
		EXPECT_EQ(v3x2x3[0][2], 2);
		EXPECT_EQ(v3x2x3[1][0], 3);
		EXPECT_EQ(v3x2x3[1][1], 4);
		EXPECT_EQ(v3x2x3[1][2], 5);
		EXPECT_EQ(it, it_end);
	}
}

TEST(TestVerilogArray, Pack) {
	Arr2 v3x2x3;
	v3x2x3[0][0] = 0;
	v3x2x3[0][1] = 1;
	v3x2x3[0][2] = 2;
	v3x2x3[1][0] = 3;
	v3x2x3[1][1] = 4;
	v3x2x3[1][2] = 5;
	vuint<18> tmps = packed(v3x2x3);
	EXPECT_EQ(tmps, 0012345);
}
