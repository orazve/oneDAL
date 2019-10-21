cmake_minimum_required(VERSION 3.0)
project(Benchmarking_Demo LANGUAGES CXX)

# Location of additional cmake modules
#
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
#

set(TOOLCHAIN_CONFIG dpcpp)

if(1)
    include(${CMAKE_SOURCE_DIR}/cmake/${TOOLCHAIN_CONFIG}.cmake)
endif()

message(STATUS "CMAKE_BINARY_DIR ...................... " ${CMAKE_BINARY_DIR})
message(STATUS "CMAKE_SOURCE_DIR ...................... " ${CMAKE_SOURCE_DIR})
message(STATUS "CMAKE_GENERATOR ....................... " ${CMAKE_GENERATOR})
message(STATUS "CMAKE_C_COMPILER_ID ................... " ${CMAKE_C_COMPILER_ID})
message(STATUS "CMAKE_CXX_COMPILER_ID ................. " ${CMAKE_CXX_COMPILER_ID})
message(STATUS "CMAKE_C_COMPILER_VERSION .............. " ${CMAKE_C_COMPILER_VERSION})
message(STATUS "CMAKE_CXX_COMPILER_VERSION ............ " ${CMAKE_CXX_COMPILER_VERSION})
message(STATUS "TOOLCHAIN_CONFIG ...................... " ${CMAKE_SOURCE_DIR}/cmake/${TOOLCHAIN_CONFIG}.cmake)

option(DAAL_USE_parallel "Use DAAL parallel" False)
option(DAAL_USE_static "Single Dynamic Library or static/dynamic" True)

include(CTest)

# If you want to self-test benchmark lib too, turn me ON
#
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Enable benchmark self-testing?")

set(BENCHMARK_DOWNLOAD_DEPENDENCIES OFF CACHE BOOL "Download dependencies?")

include(ConfigGBench)

find_package(DAAL REQUIRED)

add_subdirectory(${PROJECT_SOURCE_DIR}/src)
