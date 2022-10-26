cmake_minimum_required(VERSION 3.16)

project(RSA)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
find_package(GMP REQUIRED)

include_directories(common/include)
add_subdirectory(cmodel)
add_subdirectory(systemc)
