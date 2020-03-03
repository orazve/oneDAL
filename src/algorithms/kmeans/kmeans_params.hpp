/** file kmeans_params.hpp */
/*******************************************************************************
* Copyright 2020 Intel Corporation
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

#ifndef DALBENCH_KMEANS_PARAMS_HPP_
#define DALBENCH_KMEANS_PARAMS_HPP_

#include "fixture.hpp"

namespace dalbench {

template <typename FPType>
class GetterParamsKMeans {
public:
  using nIterations = size_t;
  using nClusters   = size_t;

  struct Params : public CommonAlgorithmParams {
    Params(const DatasetName& dataset_name,
           const NumericTableType numeric_table_type,
           const size_t num_iterations,
           const size_t num_clusters = 0)
        : CommonAlgorithmParams(dataset_name, numeric_table_type),
          num_iterations(num_iterations),
          num_clusters(num_clusters) {}

    size_t num_iterations;
    size_t num_clusters;
  };

  GetterParamsKMeans(const Params& params) : params(params) {}

  static DictionaryParams<Params> get_params() {
    return { { "Higgs:1M;Cluster:10;Iter:100",
               Params(DatasetName("higgs_1M_cluster"),
                      TableType(SyclHomogen, FPType),
                      nIterations(100u),
                      nClusters(10u)) },
             { "Higgs:1M;Cluster:100;Iter:10",
               Params(DatasetName("higgs_1M_cluster"),
                      TableType(SyclHomogen, FPType),
                      nIterations(10u),
                      nClusters(100u)) },
             { "Higgs:1M;Cluster:250;Iter:10",
               Params(DatasetName("higgs_1M_cluster"),
                      TableType(SyclHomogen, FPType),
                      nIterations(10u),
                      nClusters(250u)) },
             { "Hepmass:1M;Cluster:10;Iter:100",
               Params(DatasetName("hepmass_1M_cluster"),
                      TableType(SyclHomogen, FPType),
                      nIterations(100u),
                      nClusters(10u)) },
             { "Hepmass:1M;Cluster:100;Iter:10",
               Params(DatasetName("hepmass_1M_cluster"),
                      TableType(SyclHomogen, FPType),
                      nIterations(10u),
                      nClusters(100u)) },
             { "Hepmass:1M;Cluster:250;Iter:10",
               Params(DatasetName("hepmass_1M_cluster"),
                      TableType(SyclHomogen, FPType),
                      nIterations(10u),
                      nClusters(250u)) },
             { "Susy:4.5M;Cluster:10;Iter:100",
               Params(DatasetName("SUSY_cluster"),
                      TableType(SyclHomogen, FPType),
                      nIterations(100u),
                      nClusters(10u)) },
             { "Susy:4.5M;Cluster:100;Iter:10",
               Params(DatasetName("SUSY_cluster"),
                      TableType(SyclHomogen, FPType),
                      nIterations(10u),
                      nClusters(100u)) },
             { "Susy:4.5M;Cluster:250;Iter:10",
               Params(DatasetName("SUSY_cluster"),
                      TableType(SyclHomogen, FPType),
                      nIterations(10u),
                      nClusters(250u)) } };
  }

  Params params;
};

} // namespace dalbench

#endif // DALBENCH_KMEANS_PARAMS_HPP_
