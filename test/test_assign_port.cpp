#include "assign_port.h"
#include "verilog_int.h"
#include <gtest/gtest.h>

using namespace std;
using namespace verilog;

struct TestAssignPort : public ::testing::Test {
  CData cdata; // u8
  SData sdata; // u16
  IData idata; // u32
  QData qdata; // u64
  VlWide<4> data128;
  VlWide<8> data256;

  TestAssignPort() {
    cdata = -1;
    sdata = -1;
    idata = -1;
    qdata = -1;
    for (size_t i = 0; i < 4; i++) {
      data128[i] = -1;
    }
    for (size_t i = 0; i < 8; i++) {
      data256[i] = -1;
    }
  }
};

TEST_F(TestAssignPort, WritePrimitive) {
  write_verilator_port(cdata, 0x1234567887654321);
  write_verilator_port(sdata, 0x1234567887654321);
  write_verilator_port(idata, 0x1234567887654321);
  write_verilator_port(qdata, 0x1234567887654321);
  write_verilator_port(data128, 0x1234567887654321);
  EXPECT_EQ(cdata, 0x21);
  EXPECT_EQ(sdata, 0x4321);
  EXPECT_EQ(idata, 0x87654321);
  EXPECT_EQ(qdata, 0x1234567887654321);
  EXPECT_EQ(data128[0], 0x87654321);
  EXPECT_EQ(data128[1], 0x12345678);
  EXPECT_EQ(data128[2], 0x0);
  EXPECT_EQ(data128[3], 0x0);
}

TEST_F(TestAssignPort, WriteOneWordUnsignedVint) {
  write_verilator_port(cdata, vuint<9>(0x18a));
  write_verilator_port(sdata, vuint<9>(0x18a));
  write_verilator_port(idata, vuint<9>(0x18a));
  write_verilator_port(qdata, vuint<9>(0x18a));
  write_verilator_port(data128, vuint<9>(0x18a));
  EXPECT_EQ(cdata, 0x8a);
  EXPECT_EQ(sdata, 0x18a);
  EXPECT_EQ(idata, 0x18a);
  EXPECT_EQ(qdata, 0x18a);
  EXPECT_EQ(data128[0], 0x18a);
  EXPECT_EQ(data128[1], 0x0);
  EXPECT_EQ(data128[2], 0x0);
  EXPECT_EQ(data128[3], 0x0);

  write_verilator_port(cdata, vuint<64>(0x9abcdef00fedcba9));
  write_verilator_port(sdata, vuint<64>(0x9abcdef00fedcba9));
  write_verilator_port(idata, vuint<64>(0x9abcdef00fedcba9));
  write_verilator_port(qdata, vuint<64>(0x9abcdef00fedcba9));
  write_verilator_port(data128, vuint<64>(0x9abcdef00fedcba9));
  EXPECT_EQ(cdata, 0xa9);
  EXPECT_EQ(sdata, 0xcba9);
  EXPECT_EQ(idata, 0x0fedcba9);
  EXPECT_EQ(qdata, 0x9abcdef00fedcba9);
  EXPECT_EQ(data128[0], 0x0fedcba9);
  EXPECT_EQ(data128[1], 0x9abcdef0);
  EXPECT_EQ(data128[2], 0x0);
  EXPECT_EQ(data128[3], 0x0);
}

TEST_F(TestAssignPort, WriteUnsignedVint) {
  vuint<80> u80;
  from_hex(u80, "1234_5678_9abc_def0_1234");
  write_verilator_port(cdata, u80);
  write_verilator_port(sdata, u80);
  write_verilator_port(idata, u80);
  write_verilator_port(qdata, u80);
  write_verilator_port(data128, u80);
  EXPECT_EQ(cdata, 0x34);
  EXPECT_EQ(sdata, 0x1234);
  EXPECT_EQ(idata, 0xdef01234);
  EXPECT_EQ(qdata, 0x56789abcdef01234);
  EXPECT_EQ(data128[0], 0xdef01234);
  EXPECT_EQ(data128[1], 0x56789abc);
  EXPECT_EQ(data128[2], 0x1234);
  EXPECT_EQ(data128[3], 0x0);

  vuint<256> u256;
  from_hex(
      u256,
      "0xE07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1");
  write_verilator_port(data256, u256);
  EXPECT_EQ(data256[7], 0xE07122F2);
  EXPECT_EQ(data256[6], 0xA4A9E811);
  EXPECT_EQ(data256[5], 0x41ADE518);
  EXPECT_EQ(data256[4], 0xA2CD7574);
  EXPECT_EQ(data256[3], 0xDCB67060);
  EXPECT_EQ(data256[2], 0xB005E246);
  EXPECT_EQ(data256[1], 0x65EF532E);
  EXPECT_EQ(data256[0], 0x0CCA73E1);
}

TEST_F(TestAssignPort, ReadToUnsignedVint) {
  vuint<80> u80;

  qdata = 0x1234567890abcdef;
  read_verilator_port(u80, qdata);
  EXPECT_EQ(u80.v[0], 0x1234567890abcdef);
  EXPECT_EQ(u80.v[1], 0);

  idata = 0x12345678;
  read_verilator_port(u80, idata);
  EXPECT_EQ(u80.v[0], 0x12345678);
  EXPECT_EQ(u80.v[1], 0);

  sdata = 0x1234;
  read_verilator_port(u80, sdata);
  EXPECT_EQ(u80.v[0], 0x1234);
  EXPECT_EQ(u80.v[1], 0);

  cdata = 0x5a;
  read_verilator_port(u80, cdata);
  EXPECT_EQ(u80.v[0], 0x5a);
  EXPECT_EQ(u80.v[1], 0);
}

TEST_F(TestAssignPort, ReadPrimitiveToUnsignedVint) {
  vuint<80> vu80;

  qdata = 0x1234567890abcdef;
  read_verilator_port(vu80, qdata);
  EXPECT_EQ(vu80.v[0], 0x1234567890abcdef);
  EXPECT_EQ(vu80.v[1], 0);

  idata = 0x12345678;
  read_verilator_port(vu80, idata);
  EXPECT_EQ(vu80.v[0], 0x12345678);
  EXPECT_EQ(vu80.v[1], 0);

  sdata = 0x1234;
  read_verilator_port(vu80, sdata);
  EXPECT_EQ(vu80.v[0], 0x1234);
  EXPECT_EQ(vu80.v[1], 0);

  cdata = 0x5a;
  read_verilator_port(vu80, cdata);
  EXPECT_EQ(vu80.v[0], 0x5a);
  EXPECT_EQ(vu80.v[1], 0);
}

TEST_F(TestAssignPort, ReadArrayToShorterUnsignedVint) {
  VlWide<4> data128;
  vuint<80> vu80;

  data128[0] = 0xc527a16c;
  data128[1] = 0x6c7a4347;
  data128[2] = 0x89a0ddf1;
  data128[3] = 0x5f5c116a;

  read_verilator_port(vu80, data128);

  EXPECT_EQ(vu80.v[0], 0x6c7a4347c527a16c);
  EXPECT_EQ(vu80.v[1], 0xddf1);

  data128[0] = 0x60f0dc98;
  data128[1] = 0xf95bd86c;
  data128[2] = 0x9d77e4a6;
  data128[3] = 0xbac71422;

  read_verilator_port(vu80, data128);

  EXPECT_EQ(vu80.v[0], 0xf95bd86c60f0dc98);
  EXPECT_EQ(vu80.v[1], 0xe4a6);
}

TEST_F(TestAssignPort, ReadArrayToLongerUnsignedVint) {
  VlWide<4> data128;
  vuint<256> vu256;

  data128[0] = 0x31035200;
  data128[1] = 0x30cf606d;
  data128[2] = 0x1fe8917c;
  data128[3] = 0x0331f6bf;

  read_verilator_port(vu256, data128);

  EXPECT_EQ(vu256.v[0], 0x30cf606d31035200);
  EXPECT_EQ(vu256.v[1], 0x0331f6bf1fe8917c);
  EXPECT_EQ(vu256.v[2], 0);
  EXPECT_EQ(vu256.v[3], 0);

  data128[0] = 0xff5f012c;
  data128[1] = 0xbe53eefd;
  data128[2] = 0xe3a6e5f6;
  data128[3] = 0x4e6dba01;

  read_verilator_port(vu256, data128);

  EXPECT_EQ(vu256.v[0], 0xbe53eefdff5f012c);
  EXPECT_EQ(vu256.v[1], 0x4e6dba01e3a6e5f6);
  EXPECT_EQ(vu256.v[2], 0);
  EXPECT_EQ(vu256.v[3], 0);
}