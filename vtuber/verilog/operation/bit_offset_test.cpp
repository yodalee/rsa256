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
typedef varray<vuint<5>, 2, 3> Arr3;
struct S1 {
	Arr3     member1;
	vuint<3> member2;
	MAKE_VSTRUCT(member1, member2)
	VSTRUCT_HAS_PROCESS(S1)
};
struct S2 {
	vsint<10> member1;
	S1        member2;
	vsint<20> member3;
	MAKE_VSTRUCT(member1, member2, member3)
	VSTRUCT_HAS_PROCESS(S2)
};

TEST(TestBitOffset, StaticAssert) {
	static_assert(bit_offset_first_member_at_lsb_v<S2, 0> ==  0u);
	static_assert(bit_offset_first_member_at_lsb_v<S2, 1> == 10u);
	static_assert(bit_offset_first_member_at_lsb_v<S2, 2> == 43u);
	static_assert(bit_offset_first_member_at_msb_v<S2, 0> == 53u);
	static_assert(bit_offset_first_member_at_msb_v<S2, 1> == 20u);
	static_assert(bit_offset_first_member_at_msb_v<S2, 2> ==  0u);
}
