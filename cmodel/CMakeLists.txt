cmake_minimum_required(VERSION 3.24)

set(SRCS
  rsa.cpp
)

add_library(rsa ${SRCS})

add_executable(unit_test test_rsa.cpp)

target_link_libraries(unit_test
  rsa
  gmp
  gtest
)

