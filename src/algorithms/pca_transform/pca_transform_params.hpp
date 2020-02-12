/** file pca_transform_params.hpp */
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

#ifndef DALBENCH_PCA_TRANSFORM_PARAMS_HPP_
#define DALBENCH_PCA_TRANSFORM_PARAMS_HPP_

#include "fixture.hpp"

namespace dalbench {

namespace daal_pca           = daal::algorithms::pca;
namespace daal_pca_transform = daal::algorithms::pca::transform;

template <typename FPType>
class GetterParamsPcaTransform {
public:
  using AlgorithmType   = typename daal_pca_transform::Batch<FPType>;
  using ratioComponents = FPType;

  struct Params : public CommonAlgorithmParams {
    Params(const DatasetName &dataset_name,
           const NumericTableType numeric_table_type,
           const FPType ratio_components)
        : CommonAlgorithmParams(dataset_name, numeric_table_type),
          ratio_components(ratio_components) {}

    const FPType ratio_components;
  };

  GetterParamsPcaTransform(const Params &params) : params(params) {}

  static DictionaryParams<Params> get_params() {
    return {
      { "Higgs:1M",
        Params(DatasetName("higgs_1M"), TableType(SyclHomogen, FPType), ratioComponents(0.95)) },
      { "Epsilon:30K",
        Params(DatasetName("epsilon_30k"), TableType(SyclHomogen, FPType), ratioComponents(0.6)) }
    };
  }

  Params params;
};

} // namespace dalbench

#endif // DALBENCH_PCA_TRANSFORM_PARAMS_HPP_
