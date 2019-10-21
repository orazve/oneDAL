if (CMAKE_VERSION VERSION_LESS 3.2)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else()
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif()

# include(DownloadProject)
# download_project(PROJ                googlebenchmark
#                  GIT_REPOSITORY      https://github.com/google/benchmark.git
#                  GIT_TAG             master
#                  ${UPDATE_DISCONNECTED_IF_AVAILABLE}
# )

set(googlebenchmark_SOURCE_DIR "${PROJECT_SOURCE_DIR}/third_party/benchmark")
set(googlebenchmark_BINARY_DIR "${googlebenchmark_SOURCE_DIR}/build")

# add_subdirectory(${googlebenchmark_SOURCE_DIR} ${googlebenchmark_BINARY_DIR})

include_directories("${googlebenchmark_SOURCE_DIR}/include")

find_library(benchmark_LIBRARY NAMES "benchmark"
  PATHS ${googlebenchmark_BINARY_DIR}
  PATH_SUFFIXES lib lib64 src
  NO_DEFAULT_PATH)


message(STATUS "benchmark_LIBRARY ${benchmark_LIBRARY}")
message(STATUS "googlebenchmark_BINARY_DIR ${googlebenchmark_BINARY_DIR}")
