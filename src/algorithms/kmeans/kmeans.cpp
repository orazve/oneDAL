/** file kmeans.cpp */
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

#include "kmeans_params.hpp"

namespace dalbench {
namespace kmeans {

namespace daal_kmeans = daal::algorithms::kmeans;

template <typename DeviceType, typename FPType>
class KMeans : public GetterParamsKMeans<FPType>,
               public FixtureBatch<daal_kmeans::Batch<FPType>, DeviceType> {
public:
  using GetterParamsKMeans<FPType>::params;

  using AlgorithmType = typename daal_kmeans::Batch<FPType>;

  using AlgorithmInitType =
    typename daal_kmeans::init::Batch<FPType, daal_kmeans::init::deterministicDense>;

  KMeans(const std::string& name, const typename GetterParamsKMeans<FPType>::Params& params_in)
      : GetterParamsKMeans<FPType>(params_in),
        FixtureBatch<AlgorithmType, DeviceType>(name, params) {}

protected:
  void set_algorithm() final {
    if (params.num_clusters == 0) {
      params.num_clusters = params.num_responses;
    }

    this->algorithm_ =
      std::make_unique<AlgorithmType>(AlgorithmType(params.num_clusters, params.num_iterations));
  }

  void set_input(benchmark::State& state) final {
    auto x = params.dataset.full().x();

    AlgorithmInitType init(params.num_clusters);

    init.input.set(daal_kmeans::init::data, x);
    init.compute();

    NumericTablePtr centroids = init.getResult()->get(daal_kmeans::init::centroids);

    this->algorithm_->input.set(daal_kmeans::data, x);
    this->algorithm_->input.set(daal_kmeans::inputCentroids, centroids);
  }

  MetricParams check_result(benchmark::State& state) final {
    auto x           = params.dataset.full().x();
    auto assignments = this->algorithm_->getResult()->get(daal_kmeans::assignments);
    return Metric<MetricType::DaviesBouldinIndex>::compute_metric(x, assignments);
  }
};

DAL_BENCH_REGISTER(KMeans, CpuDevice, float);
DAL_BENCH_REGISTER(KMeans, CpuDevice, double);

#ifdef DPCPP_INTERFACES
DAL_BENCH_REGISTER(KMeans, GpuDevice, float);
DAL_BENCH_REGISTER(KMeans, GpuDevice, double);
#endif

} // namespace kmeans
} // namespace dalbench
