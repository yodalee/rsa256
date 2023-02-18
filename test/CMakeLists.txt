cmake_minimum_required(VERSION 3.16)
set(CMAKE_BUILD_TYPE Debug)
add_executable(test_dtype
	test_vint.cpp
	test_varray.cpp
	test_vstruct.cpp
#test_vunion.cpp
#test_dtype_mixed.cpp
)
target_link_libraries(test_dtype GTest::gtest_main GTest::gtest)
add_test(test_dtype test_dtype)
