cmake_minimum_required(VERSION 3.16)

project(verilog)

include_directories(${cmodel_INCLUDE_DIRS})

find_package(verilator HINTS $ENV{VERILATOR_ROOT})

set(TWO_POWER_MOD_SIM VRSATwoPowerMod)
set(MONTGOMERY_SIM VRSAMontgomery)
set(RSA_SIM VRSA)

add_executable(${TWO_POWER_MOD_SIM} RSATwoPowerMod_sim.cpp)

target_link_libraries(${TWO_POWER_MOD_SIM} PUBLIC rsa systemc glog)

verilate(${TWO_POWER_MOD_SIM} TRACE_FST SOURCES RSATwoPowerMod.sv)

add_executable(${MONTGOMERY_SIM} RSAMontgomery_sim.cpp)

target_link_libraries(${MONTGOMERY_SIM} PUBLIC rsa systemc glog)

verilate(${MONTGOMERY_SIM} TRACE_FST SOURCES RSAMontgomery.sv)

add_executable(${RSA_SIM} RSA256_sim.cpp)

target_link_libraries(${RSA_SIM} PUBLIC rsa systemc glog)

verilate(${RSA_SIM} TRACE_FST SOURCES RSA.sv)
