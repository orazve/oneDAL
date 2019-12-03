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

#  DPCPP_FOUND        -  system has DPCPP
#  DPCPP_ROOT_DIR     -  path to the DPCPP base directory
#  DPCPP_INCLUDE_DIR  -  the DPCPP include directory
#  DPCPP_LIBRARIES    -  DPCPP libraries

find_path(
  DPCPP_ROOT_DIR
  NAMES dpcpp
  PATHS $ENV{DPCPP_ROOT} $ENV{PATH}
  PATH_SUFFIXES compiler/latest/linux/bin)

get_filename_component(DPCPP_ROOT_DIR ${DPCPP_ROOT_DIR} DIRECTORY REALPATH)

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall -w -fsycl -DONEAPI_DAAL_USE_MKL_GPU_FUNC")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3 -D_GLIBCXX_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -DNDEBUG")

find_path(
  DPCPP_INCLUDE_DIR
  NAMES CL/sycl.hpp
  PATHS ${DPCPP_ROOT_DIR}/include $ENV{DPCPP_ROOT} $ENV{CPATH}
  PATH_SUFFIXES compiler/latest/linux/include)

foreach(DPCPP_LIB sycl OpenCL)
  find_library(
    DPCPP_LIBRARY
    NAMES ${DPCPP_LIB}
    PATHS ${DPCPP_ROOT_DIR}/lib $ENV{DPCPP_ROOT} $ENV{CPATH}
    PATH_SUFFIXES compiler/latest/linux/lib)

  if(DPCPP_LIBRARY)
    list(APPEND DPCPP_LIBRARIES ${DPCPP_LIBRARY})
  endif()

  unset(DPCPP_LIBRARY CACHE)

endforeach()

if(NOT DPCPP_INCLUDE_DIR MATCHES NOTFOUND)
  if(NOT DPCPP_LIBRARY MATCHES NOTFOUND)
    set(DPCPP_FOUND TRUE)
  endif()
endif()

if(NOT DEFINED DPCPP_FOUND)
  message(
    FATAL_ERROR
      "DPCPP was not found in ${DPCPP_ROOT_DIR}! Set/check DPCPP_ROOT environment variable!"
  )
else()
  message(STATUS "DPCPP_ROOT_DIR ........................ " ${DPCPP_ROOT_DIR})
  message(STATUS "DPCPP_LIBRARIES ....................... " ${DPCPP_LIBRARIES})
  message(STATUS "DPCPP_INCLUDE_DIR ..................... "
                 ${DPCPP_INCLUDE_DIR})
  add_definitions(-DDPCPP_INTERFACES)
endif()
