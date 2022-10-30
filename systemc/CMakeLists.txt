cmake_minimum_required(VERSION 3.16)

project(systemcmodel)

include_directories(${cmodel_INCLUDE_DIRS})

add_executable(model_RSA model_rsa.cpp)

set(SYSTEMC_LIBRARY rsa systemc gmp)

target_link_libraries(model_RSA ${SYSTEMC_LIBRARY})

add_executable(model_two_power_mod model_two_power_mod.cpp)

target_link_libraries(model_two_power_mod ${SYSTEMC_LIBRARY})
