#include "source.h"
#include "verilog/dtype.h"
#include "verilog/operation/pack.h"
#include <gtest/gtest.h>
#include <memory>

using namespace std;
using V64 = verilog::vuint<64>;
using A32 = verilog::varray<verilog::vuint<32>, 2>;

struct S8 {
  verilog::vuint<2> b0;
  verilog::vuint<3> b1;
  verilog::vuint<3> b2;
  MAKE_VSTRUCT(b0, b1, b2)
  VSTRUCT_HAS_PROCESS(S8)
};

struct S56 {
  S8 s8;
  verilog::varray<verilog::vuint<8>, 2> a16;
  verilog::vuint<32> v32;
  MAKE_VSTRUCT(s8, a16, v32)
  VSTRUCT_HAS_PROCESS(S56)
};

TEST(TestSource, HexSource) {
  stringstream hex("afadaba9a7a5a3a1\neacaaa8a6a4a2a0a\n");
  unique_ptr<Source<V64>> s(new VintHexSource<V64>());
  auto v64 = s->get(hex);
  EXPECT_EQ(v64.size(), 2);
  EXPECT_EQ(v64[0].value(), 0xafadaba9a7a5a3a1);
  EXPECT_EQ(v64[1].value(), 0xeacaaa8a6a4a2a0a);
}

TEST(TestSource, HexSourceArray) {
  stringstream hex("afadaba9a7a5a3a1\neacaaa8a6a4a2a0a\n");
  unique_ptr<Source<A32>> s(new VintHexSource<A32>());
  auto a32 = s->get(hex);
  EXPECT_EQ(a32.size(), 2);
  EXPECT_EQ(a32[0][1].value(), 0xafadaba9);
  EXPECT_EQ(a32[0][0].value(), 0xa7a5a3a1);
  EXPECT_EQ(a32[1][1].value(), 0xeacaaa8a);
  EXPECT_EQ(a32[1][0].value(), 0x6a4a2a0a);
}

TEST(TestSource, HexSourceStruct) {
  stringstream hex("afadaba9a7a5a3a1\neacaaa8a6a4a2a0a\n");
  unique_ptr<Source<S56>> s(new VintHexSource<S56>());
  auto s56 = s->get(hex);
  EXPECT_EQ(s56.size(), 2);

  {
    auto t = verilog::pack(s56[0]);
    EXPECT_EQ(t.value(), 0xadaba9a7a5a3a1);
  }
  {
    auto t = verilog::pack(s56[1]);
    EXPECT_EQ(t.value(), 0xcaaa8a6a4a2a0a);
  }
}

TEST(TestSource, BinarySource) {
  stringstream bin;
  verilog::SaveBinary(bin, verilog::vuint<64>(0xafadaba9a7a5a3a1));
  verilog::SaveBinary(bin, verilog::vuint<64>(0xeacaaa8a6a4a2a0a));
  unique_ptr<Source<V64>> s(new VintBinarySource<V64>());
  auto v64 = s->get(bin);
  EXPECT_EQ(v64.size(), 2);
  EXPECT_EQ(v64[0].value(), 0xafadaba9a7a5a3a1);
  EXPECT_EQ(v64[1].value(), 0xeacaaa8a6a4a2a0a);
}

TEST(TestSource, BinarySourceArray) {
  stringstream bin;
  verilog::SaveBinary(bin, verilog::vuint<64>(0xafadaba9a7a5a3a1));
  verilog::SaveBinary(bin, verilog::vuint<64>(0xeacaaa8a6a4a2a0a));
  unique_ptr<Source<A32>> s(new VintBinarySource<A32>());
  auto a32 = s->get(bin);
  EXPECT_EQ(a32.size(), 2);
  EXPECT_EQ(a32[0][0].value(), 0xa7a5a3a1);
  EXPECT_EQ(a32[0][1].value(), 0xafadaba9);
  EXPECT_EQ(a32[1][0].value(), 0x6a4a2a0a);
  EXPECT_EQ(a32[1][1].value(), 0xeacaaa8a);
}

// The layout of struct is likely to have "hole" due to alignement
// It is not recommended to read struct with LoadBinary while the source
// is created from plain hex string
TEST(TestSource, BinarySourceStruct) {
  stringstream bin;
  S56 buf0, buf1;

  // push buf0
  buf0.s8.b0 = 1;
  buf0.s8.b1 = 2;
  buf0.s8.b2 = 3;
  buf0.a16[0] = 0x5a;
  buf0.a16[1] = 0xa5;
  buf0.v32 = 0x12345678;
  verilog::SaveBinary(bin, buf0);

  // push buf1
  buf1.s8.b0 = 1;
  buf1.s8.b1 = 2;
  buf1.s8.b2 = 3;
  buf1.a16[0] = 0x3f;
  buf1.a16[1] = 0x85;
  buf1.v32 = 0x90abcdef;
  verilog::SaveBinary(bin, buf1);

  unique_ptr<Source<S56>> s(new VintBinarySource<S56>());
  auto s56 = s->get(bin);
  EXPECT_EQ(s56.size(), 2);
  EXPECT_FALSE(memcmp(&s56[0], &buf0, sizeof(S56)));
  EXPECT_FALSE(memcmp(&s56[1], &buf1, sizeof(S56)));
}
