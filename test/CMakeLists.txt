cmake_minimum_required(VERSION 3.16)
set(CMAKE_BUILD_TYPE Debug)

enable_testing()

add_executable(test_verilog_int
  test_verilog_int.cpp
)

add_executable(test_abstract_random
  test_abstract_random.cpp
)

target_link_libraries(
  test_verilog_int
  gtest_main
  gtest
)

target_link_libraries(
  test_abstract_random
  gtest_main
  gtest
)

include(GoogleTest)
gtest_discover_tests(test_verilog_int)
gtest_discover_tests(test_abstract_random)
