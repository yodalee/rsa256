cmake_minimum_required(VERSION 3.16)

project(verilog)

include_directories(${cmodel_INCLUDE_DIRS})

find_package(verilator HINTS $ENV{VERILATOR_ROOT})

set(SIM_TWO_POWER_MOD Vtwo_power_mod)

add_executable(${SIM_TWO_POWER_MOD} sim_two_power_mod.cpp)

target_link_libraries(${SIM_TWO_POWER_MOD} PUBLIC rsa systemc glog)

verilate(${SIM_TWO_POWER_MOD} TRACE_FST SOURCES two_power_mod.sv)
