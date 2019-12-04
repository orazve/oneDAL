/** file profiler.hpp */
/*******************************************************************************
* Copyright 2019 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#ifndef DALBENCH_INCLUDE_PROFILER_HPP_
#define DALBENCH_INCLUDE_PROFILER_HPP_

#include <cstring>
#include <map>
#include <vector>

#if defined(__linux__)
  #include <sys/time.h>
  #include <time.h>
#endif

#ifdef TBB_USE
  #include "tbb/enumerable_thread_specific.h"
#endif

namespace dalbench {

struct task_tls {
  static const uint64_t MAX_KERNELS = 256;
  std::map<const char*, uint64_t> kernels;
  uint64_t current_kernel = 0;
  uint64_t time_kernels[MAX_KERNELS];

  task_tls& local();
  ~task_tls();
  void clear();
};

#ifdef TBB_USE
using enumerate_type = tbb::enumerable_thread_specific<task_tls>;
#else
using enumerate_type = task_tls;
#endif

class Profiler {
public:
  static Profiler* get_instance();

  std::map<const char*, uint64_t> combine();

  enumerate_type& get_task();

  void clear();

  static uint64_t get_time();

private:
  Profiler();
  enumerate_type task;
};

} // namespace dalbench

#endif // DALBENCH_INCLUDE_PROFILER_HPP_
