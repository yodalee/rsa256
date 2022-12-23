cmake_minimum_required(VERSION 3.16)

project(systemcmodel)

include_directories(${cmodel_INCLUDE_DIRS})
set(SYSTEMC_LIBRARY rsa systemc)

add_library(RSATwoPowerMod RSATwoPowerMod.cpp)
add_executable(RSATwoPowerMod_test RSATwoPowerMod_test.cpp)
target_link_libraries(RSATwoPowerMod_test RSATwoPowerMod ${SYSTEMC_LIBRARY})

add_library(RSAMontgomery RSAMontgomery.cpp)
add_executable(RSAMontgomery_test RSAMontgomery_test.cpp)
target_link_libraries(RSAMontgomery_test RSAMontgomery ${SYSTEMC_LIBRARY})

add_library(RSA256 RSA256.cpp)
add_executable(RSA256_test RSA256_test.cpp)
target_link_libraries(RSA256_test RSA256 ${SYSTEMC_LIBRARY})
