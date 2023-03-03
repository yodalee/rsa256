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
#include "verilog/dtype.h"
#include <iostream>
using namespace std;
using namespace verilog;
typedef varray<vuint<10>, 5> Arr1;
typedef varray<vuint<5>,4, 2, 3> Arr2;
typedef varray<vuint<5>, 2, 3> Arr3;
struct S1 {
	Arr3     member1;
	vuint<3> member2;
	MAKE_VSTRUCT(member1, member2)
	VSTRUCT_HAS_PROCESS(S1)
};

TEST(TestPrint, ArrayScalarSingleDim) {
	Arr1 v10x5;
	auto it = v10x5.begin();
	for (unsigned i = 0; i < Arr1::asize; ++i, ++it) {
		*it = i;
	}

	::std::stringstream ostr;
	ostr << v10x5;
	cout << v10x5 << endl;
	EXPECT_EQ(ostr.str(), "[0,1,2,3,4]");
}

TEST(TestPrint, ArrayScalarMultiDim) {
	Arr2 v5x4x2x3;
	auto it = v5x4x2x3.begin();
	for (unsigned i = 0; i < Arr2::asize; ++i, ++it) {
		*it = i;
	}
	::std::stringstream ostr;
	ostr << v5x4x2x3;
	cout << v5x4x2x3 << endl;
	EXPECT_EQ(
		ostr.str(),
R"([
	[
		[0,1,2],
		[3,4,5]
	],
	[
		[6,7,8],
		[9,10,11]
	],
	[
		[12,13,14],
		[15,16,17]
	],
	[
		[18,19,20],
		[21,22,23]
	]
])"
	);
}

TEST(TestPrint, Struct) {
	S1 s1;
	s1.member1[0][0] = 0;
	s1.member1[0][1] = 1;
	s1.member1[0][2] = 2;
	s1.member1[1][0] = 3;
	s1.member1[1][1] = 4;
	s1.member1[1][2] = 5;
	s1.member2 = 7;
	::std::stringstream ostr;
	ostr << s1;
	cout << s1 << endl;
	EXPECT_EQ(
		ostr.str(),
R"({
	"member1": [
		[0,1,2],
		[3,4,5]
	],
	"member2": 7
})"
	);
}
