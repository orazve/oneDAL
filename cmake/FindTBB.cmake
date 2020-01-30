#===============================================================================
# Copyright 2019-2020 Intel Corporation
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

#  TBB_FOUND        -  system has TBB
#  TBB_ROOT_DIR     -  path to the TBB base directory
#  TBB_INCLUDE_DIR  -  the TBB include directory
#  TBB_LIBRARIES    -  TBB libraries

find_path(
  TBB_ROOT_DIR
  NAMES include/tbb/tbb.h
  NO_DEFAULT_PATH
  PATHS $ENV{TBBROOT})

get_filename_component(TBB_ROOT_DIR ${TBB_ROOT_DIR} DIRECTORY REALPATH)

if(NOT TBB_ROOT_DIR)
  message(FATAL_ERROR "TBB was not found!
    Set/check TBBROOT environment variable!")
endif()

find_path(
  TBB_INCLUDE_DIR
  NAMES tbb/tbb.h
  NO_DEFAULT_PATH
  PATH_SUFFIXES include
  PATHS ${TBB_ROOT_DIR} $ENV{TBBROOT})

foreach(TBB_LIB tbb tbbmalloc)
  find_library(
    TBB_LIBRARY NO_DEFAULT_PATH
    NAMES ${TBB_LIB}
    PATH_SUFFIXES lib/intel64/gcc4.4 latest/lib/intel64/gcc4.8
    PATHS ${TBB_ROOT_DIR} $ENV{LIBRARY_PATH})

  if(TBB_LIBRARY)
    list(APPEND TBB_LIBRARIES ${TBB_LIBRARY})
  endif()

  unset(TBB_LIBRARY CACHE)

endforeach()

if(NOT TBB_INCLUDE_DIR MATCHES NOTFOUND)
  if(TBB_LIBRARIES)
    set(TBB_FOUND TRUE)
  endif()
endif()

if(NOT DEFINED TBB_FOUND)
  message(
    FATAL_ERROR
      "TBB was not found in ${TBB_ROOT_DIR}! Set/check TBBROOT environment variable!"
  )
else()
  message(STATUS "TBB_ROOT_DIR .......................... " ${TBB_ROOT_DIR})
  message(STATUS "TBB_LIBRARIES ......................... " ${TBB_LIBRARIES})
  message(STATUS "TBB_INCLUDE_DIR ....................... " ${TBB_INCLUDE_DIR})
  add_definitions(-DTBB_USE)
endif()
