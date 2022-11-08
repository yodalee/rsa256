cmake_minimum_required(VERSION 3.16)

project(verilog)

include_directories(${cmodel_INCLUDE_DIRS})

find_package(verilator HINTS $ENV{VERILATOR_ROOT})

add_executable(Vdummy dummy.cpp)

target_link_libraries(Vdummy rsa systemc gmp)

verilate(Vdummy SOURCES dummy.sv)
