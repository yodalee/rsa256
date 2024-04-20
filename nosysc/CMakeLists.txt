cmake_minimum_required(VERSION 3.16)

find_package(verilator HINTS $ENV{VERILATOR_ROOT})
set(VERILATEOPT TRACE_FST INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/verilog)
set(LINKME PRIVATE glog gtest gtest_main)

set(LIB PipelineInt)
set(EXE PipelineInt_sim)
add_library(${LIB})
verilate(${LIB} ${VERILATEOPT} PREFIX V${LIB} SOURCES ${LIB}.v)
add_executable(${EXE} ${EXE}.cpp)
target_link_libraries(${EXE} ${LINKME} ${LIB})