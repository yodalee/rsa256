cmake_minimum_required(VERSION 3.24)

project(systemcmodel)

set(CMAKE_CXX_STANDARD 17)

add_executable(model_RSA model_rsa.cpp)

include_directories(${cmodel_INCLUDE_DIRS})

target_link_libraries(model_RSA
  rsa
  systemc
)

