cmake_minimum_required(VERSION 3.16)
set(CMAKE_BUILD_TYPE Debug)

enable_testing()

add_executable(unit_test
  test_verilog_int.cpp
#  test_verilog_bundle.cpp
#  test_assign_port.cpp
#  test_abstract_random.cpp
)

target_link_libraries(
  unit_test
  gtest_main
  gtest
)

include(GoogleTest)
gtest_discover_tests(unit_test)
target_include_directories(unit_test PUBLIC "/usr/share/verilator/include")
