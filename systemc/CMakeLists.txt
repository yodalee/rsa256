cmake_minimum_required(VERSION 3.16)

project(systemcmodel)

include_directories(${cmodel_INCLUDE_DIRS})

add_executable(RSA256 RSA256.cpp)

set(SYSTEMC_LIBRARY rsa systemc)

target_link_libraries(RSA256 ${SYSTEMC_LIBRARY})


add_library(RSATwoPowerMod RSATwoPowerMod.cpp)
add_executable(RSATwoPowerMod_test RSATwoPowerMod_test.cpp)
target_link_libraries(RSATwoPowerMod_test ${SYSTEMC_LIBRARY} RSATwoPowerMod)

add_library(RSAMontgomery RSAMontgomery.cpp)
add_executable(RSAMontgomery_test RSAMontgomery_test.cpp)
target_link_libraries(RSAMontgomery_test ${SYSTEMC_LIBRARY} RSAMontgomery)

