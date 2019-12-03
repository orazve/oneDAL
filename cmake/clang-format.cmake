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

set(CLANG_FORMAT_CXX_FILE_EXTENSIONS ${CLANG_FORMAT_CXX_FILE_EXTENSIONS} *.cpp
                                     *.hpp)
file(GLOB_RECURSE ALL_CXX_SOURCE_FILES ${CLANG_FORMAT_CXX_FILE_EXTENSIONS})

set(CLANG_FORMAT_EXCLUDE_PATTERNS ${CLANG_FORMAT_EXCLUDE_PATTERNS}
                                  "/CMakeFiles/" "cmake")

set(CLANG_FORMAT_BIN_NAME clang-format)

find_program(CLANG_FORMAT_EXE NAMES ${CLANG_FORMAT_BIN_NAME})

if(CLANG_FORMAT_EXE)
  add_custom_target(
    clang-format
    COMMAND ${CLANG_FORMAT_EXE} -i -style=file ${ALL_CXX_SOURCE_FILES}
    COMMENT "Formating with ${CLANG_FORMAT_EXE} ...")
else()
  message(WARNING "clang-format was not found")
endif()
