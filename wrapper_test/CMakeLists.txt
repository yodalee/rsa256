cmake_minimum_required(VERSION 3.16)

find_package(verilator HINTS $ENV{VERILATOR_ROOT})
set(VERILATEOPT TRACE INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/verilog)
set(LINKME PRIVATE glog)

add_library(RotComb)
verilate(RotComb ${VERILATEOPT} PREFIX VRotComb SOURCES Rot.sv TOP_MODULE RotComb)

add_library(RotSeq)
verilate(RotSeq ${VERILATEOPT} PREFIX VRotSeq SOURCES Rot.sv TOP_MODULE RotSeq)

set(TARGET Rot)
add_executable(${TARGET} ${TARGET}.cpp)
target_link_libraries(${TARGET} PRIVATE glog RotSeq RotComb)