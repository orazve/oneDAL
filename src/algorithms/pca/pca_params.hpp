/** file pca_params.hpp */
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

#ifndef DALBENCH_PCA_PARAMS_HPP_

  #include "fixture.hpp"

namespace dalbench {

namespace daal_pca = daal::algorithms::pca;

template <typename FPType>
class GetterParamsPcaCov {
public:
  using nComponent = FPType;

  struct Params : public CommonAlgorithmParams {
    Params(const DatasetName& dataset_name,
           const NumericTableType numeric_table_type,
           const daal_pca::ResultToComputeId compute_id,
           const FPType ratio_components = 1.0)
        : CommonAlgorithmParams(dataset_name, numeric_table_type),
          compute_id(compute_id),
          ratio_components(ratio_components) {}

    const daal_pca::ResultToComputeId compute_id;
    const FPType ratio_components;
    int n_components;
  };

  GetterParamsPcaCov(const Params& params) : params(params) {}

  static DictionaryParams<Params> get_params() {
    return { { "Higgs:1M",
               Params(DatasetName("higgs_1M"),
                      TableType(SyclHomogen, FPType),
                      daal_pca::ResultToComputeId(daal_pca::eigenvalues | daal_pca::eigenvectors |
                                                  daal_pca::variances)) },
             { "Epsilon:30K",
               Params(DatasetName("epsilon_30k"),
                      TableType(SyclHomogen, FPType),
                      daal_pca::ResultToComputeId(daal_pca::eigenvalues | daal_pca::eigenvectors |
                                                  daal_pca::variances)) } };
  }

  Params params;
};

} // namespace dalbench

#endif // DALBENCH_PCA_PARAMS_HPP_
