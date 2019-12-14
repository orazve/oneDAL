/** file fixture.hpp */
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

#ifndef DALBENCH_INCLUDE_FIXTURE_HPP_
#define DALBENCH_INCLUDE_FIXTURE_HPP_

#include <list>
#include <memory>
#include <sstream>
#include <vector>

#include "benchmark/benchmark.h"

#ifdef DPCPP_INTERFACES
  #include <CL/cl.h>

  #include <CL/sycl.hpp>

  #include "daal_sycl.h"
#else
  #include "daal.h"
#endif

#define ONEDAL_VERSION_2021_BETA_03        20191000
#define ONEDAL_VERSION_2021_BETA_03_UPDATE 20191200
#define ONEDAL_VERSION_2021_BETA_04        20200100

#include "dataset.hpp"
#include "error_types.hpp"
#include "profiler.hpp"
#include "statistics.hpp"

namespace dalbench {

struct CpuDevice {
#ifdef DPCPP_INTERFACES
  static auto get_device() {
    static auto selector = cl::sycl::cpu_selector();
    return selector;
  }
#endif
};

struct GpuDevice {
#ifdef DPCPP_INTERFACES
  static auto get_device() {
    static auto selector = cl::sycl::gpu_selector();
    return selector;
  }
#endif
};

template <typename ParamsType>
using DictionaryParams       = std::list<std::pair<std::string, ParamsType>>;
using DatasetName            = std::string;
const size_t MAX_NUM_OF_RUNS = 5; // TODO

struct CommonAlgorithmParams {
  CommonAlgorithmParams(const DatasetName& dataset_name,
                        const NumericTableType numeric_table_type,
                        const size_t num_blocks = 1)
      : dataset_name(dataset_name),
        numeric_table_type(numeric_table_type),
        num_blocks(num_blocks) {
  }

  void load_dataset() {
    auto dataset_loader = create_registered<DatasetLoader>(dataset_name);
    dataset             = dataset_loader->load(numeric_table_type, num_blocks);
    num_responses       = dataset.num_responses();
  }

  void clear_dataset() {
    dataset = Dataset();
  }

  DatasetName dataset_name;
  NumericTableType numeric_table_type;
  Dataset dataset;
  size_t num_blocks;
  size_t num_observations;
  size_t num_features;
  size_t num_responses;
};

template <typename AlgorithmType, typename DeviceType>
class Fixture : public ::benchmark::Fixture {
public:
  Fixture(CommonAlgorithmParams& params)
      : ::benchmark::Fixture(),
        common_params_(params),
        current_run_(0),
        num_runs_(MAX_NUM_OF_RUNS) {
#ifdef DPCPP_INTERFACES
    cl::sycl::device device;
    try {
      device = cl::sycl::device(DeviceType::get_device());
    }
    catch (...) {
      throw NotAvailableDevice("The device is not supported");
    }
    cl::sycl::queue queue(device);
    ctx_ = std::make_unique<daal::services::ExecutionContext>(
      daal::services::SyclExecutionContext(queue));
#else
    ctx_ =
      std::make_unique<daal::services::ExecutionContext>(daal::services::CpuExecutionContext());
#endif
  }

  void SetUp(benchmark::State& state) final {
    try {
      daal::services::Environment::getInstance()->setDefaultExecutionContext(*ctx_);
      if (current_run_ == 0) {
        common_params_.load_dataset();
      }
      set_algorithm();
      set_input();
      set_parameters();
    }
    catch (std::exception const& e) {
      state.SkipWithError(e.what());
    }
  }

  void BenchmarkCase(benchmark::State& state) final {
    current_run_++;
    run_benchmark(state);
    auto kernels_profiler = Profiler::get_instance()->combine();
    for (auto& kernel_info : kernels_profiler) {
      auto kernel_name     = kernel_info.first;
      const double time_ms = double(kernel_info.second) / 1e6;
      state.counters.insert({ kernel_name, { time_ms, benchmark::Counter::kDefaults } });
#ifdef __DEBUG__
      std::cout << "kernel: " << kernel_name << " time_ms: " << time_ms << "\n";
#endif
    }
    Profiler::get_instance()->clear();
  }

  void TearDown(benchmark::State& state) final {
    if (current_run_ == num_runs_) {
      common_params_.clear_dataset();
      current_run_ = 0;
      num_runs_    = 1;
    }

    algorithm_.reset();
  }

protected:
  virtual void run_benchmark(benchmark::State& state) {
  }

  virtual void set_algorithm() {
  }

  virtual void set_input() {
  }

  virtual void set_parameters() {
  }

protected:
  CommonAlgorithmParams& common_params_;
  std::unique_ptr<AlgorithmType> algorithm_;
  std::unique_ptr<daal::services::ExecutionContext> ctx_;

private:
  size_t num_runs_;
  size_t current_run_;
};

template <typename AlgorithmType, typename DeviceType>
class FixtureBatch : public Fixture<AlgorithmType, DeviceType> {
public:
  FixtureBatch(CommonAlgorithmParams& params) : Fixture<AlgorithmType, DeviceType>(params) {
  }

protected:
  void run_benchmark(benchmark::State& state) final {
    for (auto _ : state) {
      this->algorithm_->compute();
    }
  }
};

template <typename AlgorithmType, typename DeviceType>
class FixtureOnline : public Fixture<AlgorithmType, DeviceType> {
public:
  FixtureOnline(CommonAlgorithmParams& params) : Fixture<AlgorithmType, DeviceType>(params) {
  }

protected:
  virtual void set_input_block(const size_t block_index) = 0;

  void run_benchmark(::benchmark::State& state) final {
    for (auto _ : state) {
      for (size_t block_index = 0; block_index < this->common_params_.num_blocks; ++block_index) {
        state.PauseTiming();
        try {
          this->set_input_block(block_index);
        }
        catch (std::exception const& e) {
          state.SkipWithError(e.what());
          return;
        }
        state.ResumeTiming();
        this->algorithm_->compute();
      }

      this->algorithm_->finalizeCompute();
    }
  }
};

template <typename BenchAlg>
struct BenchmarkRegistrator {
  BenchmarkRegistrator(const std::string& name) {
    const auto& params = BenchAlg::get_params();
    for (const auto& pair_param : params) {
      const auto& key   = pair_param.first;
      const auto& param = pair_param.second;
      auto* bench =
        ::benchmark::internal::RegisterBenchmarkInternal(new BenchAlg(name + "/" + key, param));
      bench->Unit(benchmark::kMillisecond);
      bench->Iterations(1);
      bench->Repetitions(MAX_NUM_OF_RUNS);
      bench->ReportAggregatesOnly();
#ifndef __DEBUG__
      bench->ComputeStatistics("box_filter", statistics::box_filter);
      bench->ComputeStatistics("first_iteration", statistics::first_iteration);
#endif
    }
  }
};

#define DAAL_BENCH_REGISTER(BaseClass, DeviceType, FPType)                           \
  static BenchmarkRegistrator<BaseClass<DeviceType, FPType>> BENCHMARK_PRIVATE_NAME( \
    BenchmarkRegistrator)(#BaseClass "/" #DeviceType "/" #FPType)

} // namespace dalbench

#endif // DALBENCH_INCLUDE_FIXTURE_HPP_
