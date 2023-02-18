cmake_minimum_required(VERSION 3.16)
find_package(verilator HINTS $ENV{VERILATOR_ROOT})
set(TWO_POWER_MOD_SIM VRSATwoPowerMod)
set(MONTGOMERY_SIM VRSAMontgomery)
set(RSA_SIM VRSA)

add_executable(${TWO_POWER_MOD_SIM} RSATwoPowerMod_sim.cpp)
add_executable(${MONTGOMERY_SIM} RSAMontgomery_sim.cpp)
add_executable(${RSA_SIM} RSA256_sim.cpp)

target_link_libraries(${TWO_POWER_MOD_SIM} PRIVATE SystemC::systemc glog PUBLIC RSASystemCModel)
target_link_libraries(${MONTGOMERY_SIM} PRIVATE SystemC::systemc glog PUBLIC RSASystemCModel)
target_link_libraries(${RSA_SIM} PRIVATE SystemC::systemc glog PUBLIC RSASystemCModel)

verilate(${TWO_POWER_MOD_SIM} TRACE_FST SOURCES RSATwoPowerMod.sv)
verilate(${MONTGOMERY_SIM} TRACE_FST SOURCES RSAMontgomery.sv)
verilate(${RSA_SIM} TRACE_FST PREFIX VRSA SOURCES RSA_pkg.sv RSA.sv)

add_test(${TWO_POWER_MOD_SIM} ${TWO_POWER_MOD_SIM})
add_test(${MONTGOMERY_SIM} ${MONTGOMERY_SIM})
add_test(${RSA_SIM} ${RSA_SIM})
