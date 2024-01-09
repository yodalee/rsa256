
#include "VPipelineTest.h"
#include "bridge/verilator/c_dut_wrapper.h"
#include "bridge/verilator/verilator_assign.h"
#include "verilog/dtype.h"
#include <iostream>

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
TEST_F(PipelineTester, SimpleCase) {
  dut->Run(100);
  LOG(INFO) << "Done";
  EXPECT_TRUE(dut->is_pass());
}
