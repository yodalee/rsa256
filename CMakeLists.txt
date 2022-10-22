cmake_minimum_required(VERSION 3.24)

project(RSA)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
find_package(GMP REQUIRED)

add_subdirectory(test)
