cmake_minimum_required(VERSION 3.24)

project(RSA)

set(CMAKE_CXX_STANDARD 14)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
find_package(GMP REQUIRED)

add_subdirectory(cmodel)
