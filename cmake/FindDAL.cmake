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

#  DAL_FOUND        -  system has DAL
#  DAL_ROOT_DIR     -  path to the DAL base directory
#  DAL_INCLUDE_DIR  -  the DAL include directory
#  DAL_LIBRARIES    -  DAL libraries

find_path(
  DAL_ROOT_DIR
  NAMES include/daal_sycl.h
  PATHS $ENV{DAALROOT})

get_filename_component(DAL_ROOT_DIR ${DAL_ROOT_DIR} REALPATH)

if(NOT DAL_ROOT_DIR)
  message(FATAL_ERROR "oneDAL was not found in ${DAL_ROOT_DIR}!
      Set/check DAALROOT environment variable!")
endif()
set(DAL_INCLUDE_DIR ${DAL_ROOT_DIR}/include)

set(DAL_LIBRARY_SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})
set(DAL_LIBRARY_PREFIX ${CMAKE_SHARED_LIBRARY_PREFIX})
if(DAL_USE_STATIC)
  set(DAL_LIBRARY_SUFFIX ${CMAKE_STATIC_LIBRARY_SUFFIX})
  set(DAL_LIBRARY_PREFIX ${CMAKE_STATIC_LIBRARY_PREFIX})
endif()

if(DAL_USE_PARALLEL)
  set(DAL_THREAD_LIBRARY_NAME daal_thread)
  find_package(TBB REQUIRED)
else()
  set(DAL_THREAD_LIBRARY_NAME daal_sequential)
endif()

foreach(DAL_LIB daal_core ${DAL_THREAD_LIBRARY_NAME})
  find_library(
    DAL_LIBRARY
    NAMES "${DAL_LIBRARY_PREFIX}${DAL_LIB}${DAL_LIBRARY_SUFFIX}"
    PATH_SUFFIXES lib/intel64
    PATHS ${DAL_ROOT_DIR})

  if(DAL_LIBRARY)
    list(APPEND DAL_LIBRARIES ${DAL_LIBRARY})
  else()
    message(WARNING "Library ${DAL_LIB} was not found!")
  endif()

  unset(DAL_LIBRARY CACHE)

endforeach()

# Link SYCL-FPK static libraries
set(MICROLIBS daal_sycl)
find_library(
  DAL_LIBRARY
  NAMES "${DAL_LIBRARY_PREFIX}${MICROLIBS}${CMAKE_STATIC_LIBRARY_SUFFIX}"
  PATH_SUFFIXES lib/intel64
  PATHS ${DAL_ROOT_DIR})

if(DAL_LIBRARY AND DAL_USE_DPCPP_INTERFACES)
  list(APPEND DAL_LIBRARIES ${DAL_LIBRARY})
endif()

if(NOT DAL_INCLUDE_DIR MATCHES NOTFOUND)
  if(NOT DAL_LIBRARIES MATCHES NOTFOUND)
    set(DAL_FOUND TRUE)
  endif()
endif()

if(NOT DEFINED DAL_FOUND)
  message(
    FATAL_ERROR
      "oneDAL was not found in ${DAL_ROOT_DIR}! Set/check DAALROOT environment variable!"
  )
else()
  message(STATUS "DAL_ROOT_DIR .......................... " ${DAL_ROOT_DIR})
  message(STATUS "DAL_LIBRARIES ......................... " ${DAL_LIBRARIES})
  message(STATUS "DAL_INCLUDE_DIR ....................... " ${DAL_INCLUDE_DIR})
endif()
