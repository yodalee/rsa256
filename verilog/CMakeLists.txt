cmake_minimum_required(VERSION 3.16)
find_package(verilator HINTS $ENV{VERILATOR_ROOT})
set(VERILATEOPT TRACE_FST)
set(LINKME PRIVATE SystemC::systemc glog RSASystemCModel)

set(TwoPower TwoPower_sim)
set(Montgomery Montgomery_sim)
set(RSAMont RSAMont_sim)
set(RSA RSA_sim)

set(MODULES
  TwoPower
  Montgomery
  RSAMont
  RSA
)

foreach(LIB ${MODULES})
  # build library
  add_library(${LIB})
  verilate(${LIB} ${VERILATEOPT} PREFIX V${LIB} SOURCES RSA_pkg.sv ${LIB}.sv)

  foreach(EXE ${${LIB}})
    add_executable(${EXE} ${EXE}.cpp)
    target_link_libraries(${EXE} ${LINKME} ${LIB})
    add_test(V${EXE} ${EXE})
  endforeach()
endforeach()

configure_file(RSA_m.in . COPYONLY)
configure_file(RSA_c.in . COPYONLY)
