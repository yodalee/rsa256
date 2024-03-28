#include "VPortVariant.h"
#include "accessor.h"
#include "verilog/dtype.h"
#include <gtest/gtest.h>

using namespace std;
using namespace verilog;
using namespace verilog::verilator;

TEST(TestAccessorCreation, TestPortVariant) {
  ::std::unique_ptr<VPortVariant> dut = make_unique<VPortVariant>();

  // bit test
  // Note that write_port/read_port are limited to use vint
  using Bit = verilog::vuint<1>;
  ::std::unique_ptr<Accessor<Bit>> bit_in =
      make_unique<VerilatorAccessor<decltype(dut->bit_in), Bit>>(dut->bit_in);
  ::std::unique_ptr<Accessor<Bit>> bit_out =
      make_unique<VerilatorAccessor<decltype(dut->bit_out), Bit>>(dut->bit_out);
  bit_in->write(Bit(1));
  dut->eval();
  EXPECT_TRUE(bit_out->read().value());
  bit_in->write(Bit(0));
  dut->eval();
  EXPECT_FALSE(bit_out->read().value());

  // byte test
  // short test
  // int test
  // long test
  // verylong test
  // struct test
};
