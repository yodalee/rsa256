#include "verilator_assign_port.h"
#include "verilog_int.h"
#include <gtest/gtest.h>

using namespace std;
using namespace verilog;
using namespace verilog::verilator;

TEST(TestAssignPort, Read) {
  VL_IN8(ver5,5,0);
  VL_IN16(ver10,10,0);
  VL_IN(ver20,20,0);
  VL_IN64(ver40,40,0);
  VL_INW(ver90,90,0,3);
  VL_INW(ver100,100,0,4);
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
  VL_IN8(ver5,5,0);
  VL_IN16(ver10,10,0);
  VL_IN(ver20,20,0);
  VL_IN64(ver40,40,0);
  VL_INW(ver90,90,0,3);
  VL_INW(ver100,100,0,4);
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

TEST(TestAssignPort, DISABLED_ReadArray) {
};

TEST(TestAssignPort, DISABLED_WriteArray) {
};
