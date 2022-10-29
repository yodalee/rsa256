cmake_minimum_required(VERSION 3.16)

enable_testing()

add_executable(test_verilog_int
  test_verilog_int.cpp
)

target_link_libraries(
  test_verilog_int
  gtest_main
  gtest
)

include(GoogleTest)
gtest_discover_tests(test_verilog_int)
