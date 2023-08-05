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
  EXPECT_EQ(a32[0][0].value(), 0xafadaba9);
  EXPECT_EQ(a32[0][1].value(), 0xa7a5a3a1);
  EXPECT_EQ(a32[1][0].value(), 0xeacaaa8a);
  EXPECT_EQ(a32[1][1].value(), 0x6a4a2a0a);
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
