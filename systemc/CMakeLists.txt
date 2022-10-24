cmake_minimum_required(VERSION 3.16)

project(systemcmodel)

add_executable(model_RSA model_rsa.cpp)

include_directories(${cmodel_INCLUDE_DIRS})

target_link_libraries(model_RSA
  rsa
  systemc
  gmp
)

