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

find_package(Threads REQUIRED)

set(BENCHMARK_TARGET "Benchmark")

set(ALGORITHMS_LIST covariance logistic_regression kmeans pca linear_regression
                    low_order_moments)

list(SORT ALGORITHMS_LIST)

foreach(ALGORITHM ${ALGORITHMS_LIST})
  file(GLOB ALGORITHM_CPP "algorithms/${ALGORITHM}/*.cpp")
  list(APPEND ALGORITHMS_LIST_SRC ${ALGORITHM_CPP})
endforeach()

file(GLOB DATASETS_CPP datasets/*.cpp)
file(GLOB UTILS_CPP utils/*.cpp)

add_executable(${BENCHMARK_TARGET} main.cpp)

target_sources(
  ${BENCHMARK_TARGET}
  PRIVATE
  ${UTILS_CPP}
  PRIVATE
  ${DATASETS_CPP}
  PRIVATE
  ${ALGORITHMS_LIST_SRC})

target_include_directories(
  ${BENCHMARK_TARGET}
  PRIVATE ${CMAKE_SOURCE_DIR}/src/utils
  PRIVATE ${GBENCH_INCLUDE_DIR}
  PRIVATE ${DAL_INCLUDE_DIR}
  PRIVATE ${DPCPP_INCLUDE_DIR})

target_link_libraries(
  ${BENCHMARK_TARGET}
  PRIVATE benchmark
  PRIVATE ${DPCPP_LIBRARIES}
  PRIVATE ${DAL_LIBRARIES}
  PRIVATE ${UTILS_LIBRARY}
  PRIVATE ${TBB_LIBRARIES}
  PRIVATE ${CMAKE_THREAD_LIBS_INIT}
  PRIVATE ${CMAKE_DL_LIBS})

install(TARGETS ${BENCHMARK_TARGET} RUNTIME DESTINATION ${CMAKE_BINARY_DIR})
