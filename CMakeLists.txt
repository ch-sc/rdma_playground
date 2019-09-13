cmake_minimum_required(VERSION 3.14)
project(rdma)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

#add_executable(rdma_test main.cpp)

# if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
#     set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/bin/rdma_lib)
# endif()

# for GDB debugging
set(CMAKE_BUILD_TYPE Debug)

# Compiler Options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLINUX -DDEBUG -std=c++14 -Wall -O3 -Wextra ")

# the compiler flags for compiling C++ sources
MESSAGE( STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS} )

add_subdirectory(src)
#add_subdirectory(test)