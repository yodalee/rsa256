cmake_minimum_required(VERSION 3.16)
add_executable(test_rsa_cmodel test_rsa.cpp rsa.cpp)
target_link_libraries(test_rsa_cmodel GTest::gtest_main GTest::gtest)
add_test(test_rsa_cmodel test_rsa_cmodel)
