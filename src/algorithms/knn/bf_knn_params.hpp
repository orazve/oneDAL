/** file bf_knn_params.hpp */
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

#ifndef DALBENCH_BF_KNN_PARAMS_HPP_
#define DALBENCH_BF_KNN_PARAMS_HPP_

#include "fixture.hpp"

namespace dalbench {

template <typename FPType>
class GetterParamsKnnBF {
public:
  using kNeighbors = size_t;

  struct Params : public CommonAlgorithmParams {
    Params(const DatasetName& dataset_name, const NumericTableType numeric_type, const size_t num_k)
        : CommonAlgorithmParams(dataset_name, numeric_type),
          num_k(num_k) {}

    const kNeighbors num_k;
  };

  GetterParamsKnnBF(const Params& params) : params(params) {}

  static DictionaryParams<Params> get_params() {
    return { { "epsilon",
               Params(DatasetName("epsilion_50k_classifier"),
                      TableType(SyclHomogen, FPType),
                      kNeighbors(2u)) },
             { "higgs",
               Params(DatasetName("higgs_small_classifier"),
                      TableType(SyclHomogen, FPType),
                      kNeighbors(5u)) },
             { "hepmass",
               Params(DatasetName("hepmass_small_classifier"),
                      TableType(SyclHomogen, FPType),
                      kNeighbors(5u)) } };
  }

  Params params;
};

} // namespace dalbench

#endif // DALBENCH_BF_KNN_PARAMS_HPP_
