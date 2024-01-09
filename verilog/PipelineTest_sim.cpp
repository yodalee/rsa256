
#include "VPipelineTest.h"
#include "bridge/verilator/accessor.h"
#include "bridge/verilator/c_dut_wrapper.h"
#include "bridge/verilator/verilator_assign.h"
#include "verilog/dtype.h"
#include <iostream>
#include <memory>

#include <gtest/gtest.h>

using namespace std;
using namespace verilog;

using DATA = verilog::vuint<32>;
using IN = DATA;
using DUT = VPipelineTest;
using OUT = DATA;

void Raise() { std::cout << "Bad things happen\n"; }

class PipelineTester : public ::testing::Test {
protected:
  // Setup code here (optional)
  void SetUp() override {
    dut = make_unique<DUTWrapper<DUT>>("pipeline", /*dump = */ true);
  }

  // Teardown code here (optional)
  void TearDown() override {}

  ::std::unique_ptr<DUTWrapper<DUT>> dut;
};

// Example test case using the fixture
TEST_F(PipelineTester, CheckReset) {
  auto i_ready =
      make_shared<VerilatorAccessor<decltype(dut->dut->i_ready), vuint<1>>>(
          dut->dut->i_ready);
  auto o_valid =
      make_shared<VerilatorAccessor<decltype(dut->dut->o_valid), vuint<1>>>(
          dut->dut->o_valid);
  dut->Init();
  EXPECT_TRUE(i_ready->read().value());
  EXPECT_FALSE(o_valid->read().value());
}
