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
#include "verilog/dtype/varray.h"
#include "verilog/dtype/vint.h"
#include "verilog/operation/bits.h"
#include "verilog/operation/pack.h"
#include "verilog/operation/unpack.h"
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
  vuint<18> tmps = pack(v3x2x3);
  EXPECT_EQ(tmps, 0012345);
}

TEST(TestVerilogArray, Unpack) {
  vuint<256> v256;
  from_hex(v256, "D1798A3C'8326770B'91643739'7AB540E3'D21345C7'539C5996'"
                 "F9ADDDCF'A45F1DFE");
  {
    varray<vuint<32>, 8> a32_8;
    unpack(a32_8, v256);
    EXPECT_EQ(a32_8[0].value(), 0xD1798A3C);
    EXPECT_EQ(a32_8[1].value(), 0x8326770B);
    EXPECT_EQ(a32_8[2].value(), 0x91643739);
    EXPECT_EQ(a32_8[3].value(), 0x7AB540E3);
    EXPECT_EQ(a32_8[4].value(), 0xD21345C7);
    EXPECT_EQ(a32_8[5].value(), 0x539C5996);
    EXPECT_EQ(a32_8[6].value(), 0xF9ADDDCF);
    EXPECT_EQ(a32_8[7].value(), 0xA45F1DFE);
  }
  {
    varray<vuint<64>, 4> a64_4;
    unpack(a64_4, v256);
    EXPECT_EQ(a64_4[0].value(), 0xD1798A3C8326770B);
    EXPECT_EQ(a64_4[1].value(), 0x916437397AB540E3);
    EXPECT_EQ(a64_4[2].value(), 0xD21345C7539C5996);
    EXPECT_EQ(a64_4[3].value(), 0xF9ADDDCFA45F1DFE);
  }
}
