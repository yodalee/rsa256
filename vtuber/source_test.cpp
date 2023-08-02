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

TEST(TestSource, LineSource) {
  unique_ptr<Source<V64>> s(new VintLineSource<V64>("source_test_line.txt"));
  auto v64_opt = s->get();
  EXPECT_TRUE(v64_opt.has_value());
  auto v64 = v64_opt.value();
  EXPECT_EQ(v64.size(), 2);
  EXPECT_EQ(v64[0].value(), 0xafadaba9a7a5a3a1);
  EXPECT_EQ(v64[1].value(), 0xeacaaa8a6a4a2a0a);
}

TEST(TestSource, LineSourceArray) {
  unique_ptr<Source<A32>> s(new VintLineSource<A32>("source_test_line.txt"));
  auto a32_opt = s->get();
  EXPECT_TRUE(a32_opt.has_value());
  auto a32 = a32_opt.value();
  EXPECT_EQ(a32.size(), 2);
  EXPECT_EQ(a32[0][0].value(), 0xafadaba9);
  EXPECT_EQ(a32[0][1].value(), 0xa7a5a3a1);
  EXPECT_EQ(a32[1][0].value(), 0xeacaaa8a);
  EXPECT_EQ(a32[1][1].value(), 0x6a4a2a0a);
}

TEST(TestSource, LineSourceStruct) {
  unique_ptr<Source<S56>> s(new VintLineSource<S56>("source_test_line.txt"));
  auto s56_opt = s->get();
  EXPECT_TRUE(s56_opt.has_value());
  auto s56 = s56_opt.value();
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
