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
using namespace std;
using namespace verilog;
typedef varray<vuint<10>, 5> Arr1;
typedef varray<vuint<3>, 2, 3> Arr2;

DEFINE_VSTRUCT(S1)
VSTRUCT_MEMBER(VTYPE(Arr2),     member1);
VSTRUCT_MEMBER(VTYPE(vuint<3>), member2);
END_DEFINE_VSTRUCT(S1)

TEST(TestVerilogStruct, Pack) {
	S1 s1;
	static_assert(is_vstruct_v<S1>);
	static_assert(S1::bits() == 21);
	s1.member1[0][0] = 6;
	s1.member1[0][1] = 5;
	s1.member1[0][2] = 4;
	s1.member1[1][0] = 3;
	s1.member1[1][1] = 2;
	s1.member1[1][2] = 1;
	s1.member2 = 0;
	vuint<21> tmp = s1.Packed();
	EXPECT_EQ(tmp, 06543210);
}
