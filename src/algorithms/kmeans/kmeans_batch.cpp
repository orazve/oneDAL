/** file kmeans.cpp */
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

#include "fixture.hpp"

namespace dalbench {
namespace kmeans {

namespace daal_kmeans = daal::algorithms::kmeans;

template <typename DeviceType, typename FPType>
class KMeansBatch : public FixtureBatch<daal_kmeans::Batch<FPType>, DeviceType> {
public:
  using AlgorithmType = typename daal_kmeans::Batch<FPType>;
  using AlgorithmInitType =
    typename daal_kmeans::init::Batch<FPType, daal_kmeans::init::randomDense>;
  using AlgorithmParamType = typename AlgorithmType::ParameterType;
  using nIterations        = size_t;
  using nClusters          = size_t;

  struct KMeansParams : public CommonAlgorithmParams {
    KMeansParams(const DatasetName& dataset_name,
                 const NumericTableType numeric_table_type,
                 const size_t num_iterations,
                 const size_t num_clusters = 0)
        : CommonAlgorithmParams(dataset_name, numeric_table_type),
          num_iterations(num_iterations),
          num_clusters(num_clusters) {}

    size_t num_iterations;
    size_t num_clusters;
  };

  using DictionaryAlgParams = DictionaryParams<KMeansParams>;

  KMeansBatch(const std::string& name, const KMeansParams& params)
      : params_(params),
        FixtureBatch<AlgorithmType, DeviceType>(params_) {
    this->SetName(name.c_str());
  }

  static DictionaryParams<KMeansParams> get_params() {
    return { { "Higgs:1M;Cluster:10",
               KMeansParams(DatasetName("higgs_1M_cluster"),
                            TableType(SyclHomogen, FPType),
                            nIterations(100u),
                            nClusters(10u)) },
             { "Higgs:1M;Cluster:100",
               KMeansParams(DatasetName("higgs_1M_cluster"),
                            TableType(SyclHomogen, FPType),
                            nIterations(10u),
                            nClusters(100u)) },
             { "Higgs:1M;Cluster:250",
               KMeansParams(DatasetName("higgs_1M_cluster"),
                            TableType(SyclHomogen, FPType),
                            nIterations(10u),
                            nClusters(250u)) },
             { "Hepmass:1M;Cluster:10",
               KMeansParams(DatasetName("hepmass_1M_cluster"),
                            TableType(SyclHomogen, FPType),
                            nIterations(100u),
                            nClusters(10u)) },
             { "Hepmass:1M;Cluster:100",
               KMeansParams(DatasetName("hepmass_1M_cluster"),
                            TableType(SyclHomogen, FPType),
                            nIterations(10u),
                            nClusters(100u)) },
             { "Hepmass:1M;Cluster:250",
               KMeansParams(DatasetName("hepmass_1M_cluster"),
                            TableType(SyclHomogen, FPType),
                            nIterations(10u),
                            nClusters(250u)) },
             { "Susy:4.5M;Cluster:10",
               KMeansParams(DatasetName("SUSY_cluster"),
                            TableType(SyclHomogen, FPType),
                            nIterations(100u),
                            nClusters(10u)) },
             { "Susy:4.5M;Cluster:100",
               KMeansParams(DatasetName("SUSY_cluster"),
                            TableType(SyclHomogen, FPType),
                            nIterations(10u),
                            nClusters(100u)) },
             { "Susy:4.5M;Cluster:250",
               KMeansParams(DatasetName("SUSY_cluster"),
                            TableType(SyclHomogen, FPType),
                            nIterations(10u),
                            nClusters(250u)) } };
  }

protected:
  void set_algorithm() final {
    if (params_.num_clusters == 0) {
      params_.num_clusters = params_.num_responses;
    }

    this->algorithm_ =
      std::make_unique<AlgorithmType>(AlgorithmType(params_.num_clusters, params_.num_iterations));
  }

  void set_input() final {
    auto x = params_.dataset.full().x();

    AlgorithmInitType init(params_.num_clusters);

    init.input.set(daal_kmeans::init::data, x);
    init.compute();

    NumericTablePtr centroids = init.getResult()->get(daal_kmeans::init::centroids);

    this->algorithm_->input.set(daal_kmeans::data, x);
    this->algorithm_->input.set(daal_kmeans::inputCentroids, centroids);
  }

  MetricParams check_result() final {
    auto x = params_.dataset.full().x();

    auto assignments = this->algorithm_->getResult()->get(daal_kmeans::assignments);

    return Metric<MetricType::DaviesBouldinIndex>::compute_metric(x, assignments);
  }

private:
  KMeansParams params_;
};

DAAL_BENCH_REGISTER(KMeansBatch, CpuDevice, float);
DAAL_BENCH_REGISTER(KMeansBatch, CpuDevice, double);

#ifdef DPCPP_INTERFACES
DAAL_BENCH_REGISTER(KMeansBatch, GpuDevice, float);
DAAL_BENCH_REGISTER(KMeansBatch, GpuDevice, double);
#endif

} // namespace kmeans
} // namespace dalbench
