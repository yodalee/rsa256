// Direct include
// C system headers
// C++ standard library headers
#include <array>
#include <functional>
#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>
// Other libraries' .h files.
#include <gtest/gtest.h>
// Your project's .h files.
#include "verilog/dtype.h"
using namespace std;
using namespace verilog;
typedef varray<vuint<10>, 5> Arr1;
typedef varray<vuint<3>, 2, 3> Arr2;

struct S1 {
	Arr2     member1;
	vuint<3> member2;
	MAKE_VSTRUCT(member1, member2)
	VSTRUCT_HAS_PROCESS(S1)
};

TEST(TestVerilogStruct, Pack) {
	S1 s1;
	static_assert(is_vstruct_v<S1>);
	static_assert(bits<S1>() == 21);
	s1.member1[0][0] = 6;
	s1.member1[0][1] = 5;
	s1.member1[0][2] = 4;
	s1.member1[1][0] = 3;
	s1.member1[1][1] = 2;
	s1.member1[1][2] = 1;
	s1.member2 = 0;
	vuint<21> tmp = pack(s1);
	EXPECT_EQ(tmp, 06543210);
}
