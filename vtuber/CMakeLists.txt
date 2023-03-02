cmake_minimum_required(VERSION 3.16)
project(vtuber)
set(CMAKE_CXX_STANDARD 17)
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-sign-compare")
endif(MSVC)

# find_package(glog REQUIRED)
find_package(verilator REQUIRED)
find_package(GTest REQUIRED)
find_package(SystemCLanguage CONFIG REQUIRED)
find_package(Boost REQUIRED)
set(CMAKE_BUILD_TYPE Debug)
message(${VERILATOR_ROOT})
include_directories(. ${VERILATOR_ROOT}/include)

add_executable(dtype_test
	verilog/dtype/vint_test.cpp
	verilog/dtype/varray_test.cpp
	verilog/dtype/vstruct_test.cpp
#test_vunion.cpp
#test_dtype_mixed.cpp
)
add_executable(abstract_random_test
	abstract_random_test.cpp
)
add_executable(verilator_assign_port_test
	bridge/verilator/verilator_assign_test.cpp
)

foreach(target_test IN ITEMS dtype_test abstract_random_test verilator_assign_port_test)
	target_link_libraries(${target_test} PRIVATE GTest::gtest_main GTest::gtest)
	add_test(${target_test} ${target_test})
endforeach()
