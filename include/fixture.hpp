/** file fixture.hpp */
/*******************************************************************************
* Copyright 2019-2020 Intel Corporation
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
#include <string>
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
#define ONEDAL_VERSION_2021_BETA_05        20200200

#include "dataset.hpp"
#include "devices.hpp"
#include "error_types.hpp"
#include "metrics.hpp"
#include "profiler.hpp"
#include "statistics.hpp"

namespace dalbench {

template <typename ParamsType>
using DictionaryParams = std::list<std::pair<std::string, ParamsType>>;
using DatasetName      = std::string;

const size_t MAX_NUM_OF_RUNS = 5; // TODO
const size_t NUMBER_OF_BLOCK = 4; // TODO

struct CommonAlgorithmParams {
  CommonAlgorithmParams(const DatasetName& dataset_name,
                        const NumericTableType numeric_table_type,
                        const size_t num_blocks = 1)
      : dataset_name(dataset_name),
        numeric_table_type(numeric_table_type),
        num_blocks(num_blocks),
        num_observations(0),
        num_features(0),
        num_responses(0) {}

  void load_dataset() {
    auto dataset_loader = create_registered<DatasetLoader>(dataset_name);
    dataset             = dataset_loader->load(numeric_table_type, num_blocks);
    num_responses       = dataset.num_responses();
    num_features        = dataset.num_features();
  }

  void clear_dataset() {
    dataset.clear();
  }

  DatasetName dataset_name;
  NumericTableType numeric_table_type;
  Dataset dataset;
  size_t num_blocks;
  size_t num_observations;
  size_t num_features;
  size_t num_responses;
};

class FixtureCommon : public ::benchmark::Fixture {
public:
  FixtureCommon(const std::string& name, CommonAlgorithmParams& params)
      : ::benchmark::Fixture(),
        common_params_(params),
        current_run_(0),
        num_runs_(MAX_NUM_OF_RUNS) {
    this->SetName(name.c_str());
  }

  void SetUp(benchmark::State& state) final {
    try {
      if (current_run_ == 0) {
        set_library_params(state);
        common_params_.load_dataset();
        set_algorithm();
        set_input(state);
        set_parameters();
      }
    }
    catch (std::exception const& e) {
      state.SkipWithError(e.what());
    }
  }

  void BenchmarkCase(benchmark::State& state) final {
    current_run_++;
    Profiler::get_instance()->clear();

    run_benchmark(state);
    auto kernels_profiler = Profiler::get_instance()->combine();
    for (auto& kernel_info : kernels_profiler) {
      auto kernel_name     = std::string("kernel:") + kernel_info.first;
      const double time_ms = double(kernel_info.second) / 1e6;
      state.counters.insert({ kernel_name, { time_ms, benchmark::Counter::kDefaults } });
    }
    Profiler::get_instance()->clear();
  }

  void TearDown(benchmark::State& state) final {
    if (current_run_ == num_runs_) {
      auto metric = check_result(state);
      state.counters.insert({ metric.name, { metric.value, benchmark::Counter::kDefaults } });
      free_input(state);
      common_params_.clear_dataset();
    }
  }

protected:
  virtual void run_benchmark(benchmark::State& state) {}

  virtual void set_library_params(benchmark::State& state) {}
  virtual void set_algorithm() {}
  virtual void set_input(benchmark::State& state) {}
  virtual void free_input(benchmark::State& state) {}
  virtual void set_parameters() {}

  virtual MetricParams check_result(benchmark::State& state) {
    return MetricParams{ "NoMetric", std::nan("0") };
  }

protected:
  CommonAlgorithmParams& common_params_;

private:
  size_t num_runs_;
  size_t current_run_;
};

template <typename AlgorithmType, typename DeviceType>
class FixtureOneDAL : public FixtureCommon {
public:
  FixtureOneDAL(const std::string& name, CommonAlgorithmParams& params)
      : FixtureCommon(name, params) {}

protected:
  void set_library_params(benchmark::State& state) final {
#ifdef DPCPP_INTERFACES
    cl::sycl::device device = cl::sycl::device(DeviceType::get_device());
    cl::sycl::queue queue(device);
    auto ctx = daal::services::ExecutionContext(daal::services::SyclExecutionContext(queue));
#else
    auto ctx = daal::services::ExecutionContext(daal::services::CpuExecutionContext());
#endif
    daal::services::Environment::getInstance()->setDefaultExecutionContext(ctx);
  }

  void free_input(benchmark::State& state) override {
    algorithm_.reset();
  }

protected:
  std::unique_ptr<AlgorithmType> algorithm_;
};

template <typename AlgorithmType, typename DeviceType>
class FixtureBatch : public FixtureOneDAL<AlgorithmType, DeviceType> {
public:
  FixtureBatch(const std::string& name, CommonAlgorithmParams& params)
      : FixtureOneDAL<AlgorithmType, DeviceType>(name, params) {}

protected:
  void run_benchmark(benchmark::State& state) final {
    for (auto _ : state) {
      this->algorithm_->compute();
    }
  }
};

template <typename AlgorithmType, typename DeviceType>
class FixtureOnline : public FixtureOneDAL<AlgorithmType, DeviceType> {
public:
  FixtureOnline(const std::string& name, CommonAlgorithmParams& params)
      : FixtureOneDAL<AlgorithmType, DeviceType>(name, params) {
    this->common_params_.num_blocks = NUMBER_OF_BLOCK;
  }

protected:
  virtual void set_input_block(benchmark::State& state, const size_t block_index) = 0;

  void run_benchmark(::benchmark::State& state) final {
    for (auto _ : state) {
      for (size_t block_index = 0; block_index < this->common_params_.num_blocks; ++block_index) {
        state.PauseTiming();
        try {
          this->set_input_block(state, block_index);
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
      bench->ComputeStatistics("box_filter", statistics::box_filter);
      bench->ComputeStatistics("first_iteration", statistics::first_iteration);
    }
  }
};

#define DAL_BENCH_REGISTER(BaseClass, DeviceType, FPType)                            \
  static BenchmarkRegistrator<BaseClass<DeviceType, FPType>> BENCHMARK_PRIVATE_NAME( \
    BenchmarkRegistrator)(#BaseClass "/Intel/" #DeviceType "/" #FPType)

} // namespace dalbench

#endif // DALBENCH_INCLUDE_FIXTURE_HPP_
