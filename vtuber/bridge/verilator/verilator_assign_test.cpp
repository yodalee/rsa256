#include "verilator_assign.h"
#include "verilog/dtype.h"
#include <gtest/gtest.h>

using namespace std;
using namespace verilog;
using namespace verilog::verilator;

TEST(TestAssignPort, Read) {
  VL_IN8(ver5, 5, 0);
  VL_IN16(ver10, 10, 0);
  VL_IN(ver20, 20, 0);
  VL_IN64(ver40, 40, 0);
  VL_INW(ver90, 90, 0, 3);
  VL_INW(ver100, 100, 0, 4);
  vuint<5> v5;
  vuint<10> v10;
  vuint<20> v20;
  vuint<40> v40;
  vuint<90> v90;
  vuint<100> v100;
  ver5 = 3;
  ver10 = 1000;
  ver20 = 1000000;
  ver40 = 0xab12345678u;
  ver90.m_storage[0] = 0x12345678u;
  ver90.m_storage[1] = 0x55665566u;
  ver90.m_storage[2] = 0xfu;
  ver100.m_storage[0] = 0x12345678u;
  ver100.m_storage[1] = 0x55665566u;
  ver100.m_storage[2] = 0xffffffffu;
  ver100.m_storage[3] = 0x1u;

  read_port(v5, ver5);
  read_port(v10, ver10);
  read_port(v20, ver20);
  read_port(v40, ver40);
  read_port(v90, ver90);
  read_port(v100, ver100);

  EXPECT_EQ(v5, 3);
  EXPECT_EQ(v10, 1000);
  EXPECT_EQ(v20, 1000000);
  EXPECT_EQ(v40, 0xab12345678llu);
  EXPECT_EQ(to_hex(v90), "F5566556612345678");
  EXPECT_EQ(to_hex(v100), "1FFFFFFFF5566556612345678");
};

TEST(TestAssignPort, Write) {
  VL_IN8(ver5, 5, 0);
  VL_IN16(ver10, 10, 0);
  VL_IN(ver20, 20, 0);
  VL_IN64(ver40, 40, 0);
  VL_INW(ver90, 90, 0, 3);
  VL_INW(ver100, 100, 0, 4);
  vuint<5> v5;
  vuint<10> v10;
  vuint<20> v20;
  vuint<40> v40;
  vuint<90> v90;
  vuint<100> v100;
  v5 = 3;
  v10 = 1000;
  v20 = 1000000;
  v40 = 0xab12345678llu;
  from_hex(v90, "f_55665566_12345678");
  from_hex(v100, "1_ffffffff_55665566_12345678");
  write_port(ver5, v5);
  write_port(ver10, v10);
  write_port(ver20, v20);
  write_port(ver40, v40);
  write_port(ver90, v90);
  write_port(ver100, v100);
  EXPECT_EQ(ver5, 3);
  EXPECT_EQ(ver10, 1000);
  EXPECT_EQ(ver20, 1000000);
  EXPECT_EQ(ver40, 0xab12345678u);
  EXPECT_EQ(ver90.m_storage[0], 0x12345678u);
  EXPECT_EQ(ver90.m_storage[1], 0x55665566u);
  EXPECT_EQ(ver90.m_storage[2], 0xfu);
  EXPECT_EQ(ver100.m_storage[0], 0x12345678u);
  EXPECT_EQ(ver100.m_storage[1], 0x55665566u);
  EXPECT_EQ(ver100.m_storage[2], 0xffffffffu);
  EXPECT_EQ(ver100.m_storage[3], 0x1u);
};

TEST(TestAssignPort, ReadArray){
  // source
  VL_INW(ver100, 100, 0, 4);
  ver100.m_storage[0] = 0x12345678u;
  ver100.m_storage[1] = 0x55665566u;
  ver100.m_storage[2] = 0xffffffffu;
  ver100.m_storage[3] = 0x1u;
  // destination
  varray<vuint<20>, 5> a20_5;
  varray<vuint<25>, 2, 2> a25_2_2;
  read_port(a20_5, ver100);
  read_port(a25_2_2, ver100);
  EXPECT_EQ(a20_5[4], 0x1ffff);
  EXPECT_EQ(a20_5[3], 0xffff5);
  EXPECT_EQ(a20_5[2], 0x56655);
  EXPECT_EQ(a20_5[1], 0x66123);
  EXPECT_EQ(a20_5[0], 0x45678);
  // python
  // b = "000" + bin(0x1ffffffff5566556612345678)[2:]
  // [hex(int(b[i:i+25], 2)) for i in range(0, len(b), 25)]
  EXPECT_EQ(a25_2_2[1][1], 0x3fffff);
  EXPECT_EQ(a25_2_2[1][0], 0x1ffd559);
  EXPECT_EQ(a25_2_2[0][1], 0x12ab309);
  EXPECT_EQ(a25_2_2[0][0], 0x345678);
};


TEST(TestAssignPort, DISABLED_WriteArrayToNonWide){
  typedef varray<vuint<15>, 4> A60;
  typedef varray<vuint<5>, 2, 2> A20;
  // source
  A60 a60;
  A20 a20;
  // destination
  VL_IN(ver20, 20, 0);
  VL_IN64(ver60, 60, 0);

  a60[0] = 0x0123;
  a60[1] = 0x4567;
  a60[2] = 0x89ab;
  a60[3] = 0xcdef;

  auto it = a20.begin();
  for (unsigned i = 0; i < A20::asize; ++i, ++it) {
    *it = (1 << i);
  }

  // write_port(ver60, a60)
  // write_port(ver20, a20);
  // python [bin(x)[2:][-15:].rjust(15,'0') for x in [0x0123, 0x4567, 0x89ab, 0xcdef]]
  EXPECT_EQ(ver60, 0x247159c4d5cdef);
  EXPECT_EQ(ver20, 0b00001000100010001000);
};

TEST(TestAssignPort, DISABLED_WriteArrayToWide){
  // source
  varray<vuint<20>, 5> a20_5;
  varray<vuint<25>, 2, 2> a25_2_2;
  // destination
  VL_INW(ver100, 100, 0, 4);
  // test
  a20_5[0] = 0x69f9e;
  a20_5[1] = 0xad576;
  a20_5[2] = 0x1e167;
  a20_5[3] = 0x8dcd2;
  a20_5[4] = 0x28e00;
  // write_port(ver100, a20_5);
  EXPECT_EQ(ver100.m_storage[0], 0xcd228e00u);
  EXPECT_EQ(ver100.m_storage[1], 0x61e1678du);
  EXPECT_EQ(ver100.m_storage[2], 0x9f9ead57u);
  EXPECT_EQ(ver100.m_storage[3], 0x6u);

  a25_2_2[0][0] = 0x345678;
  a25_2_2[0][1] = 0x1559848;
  a25_2_2[1][0] = 0x1feaacc;
  a25_2_2[1][1] = 0x1ffffff;
  // write_port(ver100, a25_2_2);
  ver100.m_storage[0] = 0x12345678u;
  ver100.m_storage[1] = 0x55665566u;
  ver100.m_storage[2] = 0xffffffffu;
  ver100.m_storage[3] = 0x1u;
};
