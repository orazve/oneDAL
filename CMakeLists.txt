#===============================================================================
# Copyright 2019 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#===============================================================================

cmake_minimum_required(VERSION 3.6)

option(DAL_USE_DPCPP_INTERFACES "Use DAL DPC++ interfaces" ON)

if(DAL_USE_DPCPP_INTERFACES)
  find_program(
    DPCPP_COMPILER
    NAMES dpcpp
    PATHS $ENV{DPCPP_ROOT} $ENV{PATH}
    PATH_SUFFIXES compiler/latest/linux/bin)

  if(NOT DPCPP_COMPILER)
    message(FATAL_ERROR "Could not find DPCPP Compiler")
  endif()
  set(CMAKE_CXX_COMPILER "${DPCPP_COMPILER}")
endif()

project(Benchmark LANGUAGES CXX)

option(KERNEL_PROFILER "Use kernel profiler" OFF)
option(DAL_USE_PARALLEL "Use DAL parallel version" ON)
option(DAL_USE_STATIC "Use DAL static version" ON)
option(BENCHMARK_DOWNLOAD_DEPENDENCIES "Download dependencies" OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

if(DAL_USE_DPCPP_INTERFACES)
  find_package(DPCPP REQUIRED)
endif()

set(CMAKE_CXX_STANDARD 14)

if(CMAKE_BUILD_TYPE)
  if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_definitions(-D__DEBUG__)
  endif()
else()
  set(CMAKE_BUILD_TYPE "Release")
endif()

if(KERNEL_PROFILER)
  add_definitions(-DKERNEL_PROFILER)
endif()

message(STATUS "CMAKE_BUILD_TYPE ...................... " ${CMAKE_BUILD_TYPE})
message(STATUS "CMAKE_BINARY_DIR ...................... " ${CMAKE_BINARY_DIR})
message(STATUS "CMAKE_SOURCE_DIR ...................... " ${CMAKE_SOURCE_DIR})
message(STATUS "CMAKE_GENERATOR ....................... " ${CMAKE_GENERATOR})
message(STATUS "CMAKE_CXX_COMPILER_VERSION ............ "
               ${CMAKE_CXX_COMPILER_VERSION})
message(STATUS "KERNEL_PROFILER ....................... " ${KERNEL_PROFILER})
message(STATUS "DAL_USE_DPCPP_INTERFACES .............. "
               ${DAL_USE_DPCPP_INTERFACES})
message(STATUS "DAL_USE_PARALLEL ...................... " ${DAL_USE_PARALLEL})
message(STATUS "DAL_USE_STATIC ........................ " ${DAL_USE_STATIC})

find_package(DAL REQUIRED)

include_directories(include)

set(THIRDPARTY_INSTALL_DIR ${CMAKE_BINARY_DIR}/third-party)

add_subdirectory(third-party)
add_subdirectory(src)

include(${CMAKE_MODULE_PATH}/clang-format.cmake)
