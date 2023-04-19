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

typedef varray<vuint<64>, 5> Arr3;
typedef vuint<8> C;

struct S2 {
  Arr3 arr;
  C c;
  MAKE_VSTRUCT(arr, c)
  VSTRUCT_HAS_PROCESS(S2)
};

TEST(TestVerilogStruct, AssignValue) {
  S2 s2;
  static_assert(is_vstruct_v<S2>);
  static_assert(bits<S2>() == 328);
  for (size_t i = 0; i < 5; i++) {
    s2.arr[i] = 0;
  }
  s2.c = 0;
  EXPECT_EQ(s2.arr.v[0].v[0], 0);
  EXPECT_EQ(s2.arr.v[1].v[0], 0);
  EXPECT_EQ(s2.arr.v[2].v[0], 0);
  EXPECT_EQ(s2.arr.v[3].v[0], 0);
  EXPECT_EQ(s2.arr.v[4].v[0], 0);
}
