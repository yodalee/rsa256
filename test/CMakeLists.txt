cmake_minimum_required(VERSION 3.16)
set(CMAKE_BUILD_TYPE Debug)

enable_testing()

add_executable(test_dtype
  test_vint.cpp
  test_varray.cpp
#  test_vstruct.cpp
#  test_vunion.cpp
#  test_dtype_mixed.cpp
)
# add_executable(test_???
#  test_assign_port.cpp
#  test_abstract_random.cpp
# )

target_link_libraries(
  test_dtype
  gtest_main
  gtest
)

include(GoogleTest)
gtest_discover_tests(test_dtype)
# target_include_directories(test_dtype PUBLIC "/usr/share/verilator/include")
