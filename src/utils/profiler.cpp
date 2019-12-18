/** file profiler.cpp */
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

#ifdef DPCPP_INTERFACES
  #include <CL/cl.h>

  #include <CL/sycl.hpp>
#endif

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

#include "profiler.hpp"

namespace dalbench {

task_tls& task_tls::local() {
  return *this;
}

task_tls::~task_tls() {
  clear();
}

void task_tls::clear() {
  current_kernel = 0;
  kernels.clear();
}

Profiler* Profiler::get_instance() {
  static Profiler instance;
  return &instance;
}

Profiler::Profiler() {}

std::map<const char*, uint64_t> Profiler::combine() {
#ifdef TBB_USE
  auto res = task.combine([](task_tls x, task_tls y) {
    task_tls res;
    res.kernels = x.kernels;
    auto ym     = y.kernels;

    for (auto y_i : ym) {
      auto it = res.kernels.find(y_i.first);

      if (it == res.kernels.end()) {
        res.kernels.insert(y_i);
      }
      else {
        it->second = std::max(it->second, y_i.second);
      }
    }

    return x;
  });
  return res.kernels;
#else
  return task.kernels;
#endif
}

enumerate_type& Profiler::get_task() {
  return task;
}

void Profiler::clear() {
  task.clear();
  task = enumerate_type(task_tls());
}

uint64_t Profiler::get_time() {
#if defined(__linux__)
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  return t.tv_sec * 1000000000 + t.tv_nsec;
#else
  #error OS other than Linux are not supported
#endif
}

} // namespace dalbench

#ifdef KERNEL_PROFILER

namespace daal {
namespace internal {

class ProfilerTask {
public:
  ProfilerTask(const char* task_name);
  ~ProfilerTask();

private:
  const char* _task_name;
};

class Profiler {
public:
  static ProfilerTask startTask(const char* task_name);
  static void endTask(const char* task_name);
};

ProfilerTask Profiler::startTask(const char* task_name) {
  const uint64_t ns_start = dalbench::Profiler::get_time();
  auto& task_local        = dalbench::Profiler::get_instance()->get_task().local();
  task_local.time_kernels[task_local.current_kernel] = ns_start;
  task_local.current_kernel++;
  return daal::internal::ProfilerTask(task_name);
}

void Profiler::endTask(const char* task_name) {
  const uint64_t ns_end = dalbench::Profiler::get_time();
  auto& task_local      = dalbench::Profiler::get_instance()->get_task().local();
  task_local.current_kernel--;
  const uint64_t times = ns_end - task_local.time_kernels[task_local.current_kernel];

  auto it = task_local.kernels.find(task_name);
  if (it == task_local.kernels.end()) {
    task_local.kernels.insert({ task_name, times });
  }
  else {
    it->second += times;
  }
}

ProfilerTask::ProfilerTask(const char* task_name) : _task_name(task_name) {}

ProfilerTask::~ProfilerTask() {
  Profiler::endTask(_task_name);
}

} // namespace internal
} // namespace daal

#endif // KERNEL_PROFILER
