cmake_minimum_required(VERSION 3.16)

project(cmodel)

set(SRCS
  rsa.cpp
)

add_library(rsa ${SRCS})

add_executable(test_rsa_cmodel test_rsa.cpp)

target_link_libraries(test_rsa_cmodel
  rsa
  pthread
  gtest
)

# Expose cmodel public includes
set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
