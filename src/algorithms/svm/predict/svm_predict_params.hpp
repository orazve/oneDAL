/** file svm_predict_params.hpp */
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

#ifndef DALBENCH_PREDICT_SVM_PARAMS_HPP_
#define DALBENCH_PREDICT_SVM_PARAMS_HPP_

#include "fixture.hpp"

namespace dalbench {

template <typename FPType>
class GetterParamsSvmPredict {
public:
  enum class KernelType {
    linear,
    rbf,
  };

  struct Params : public CommonAlgorithmParams {
    Params(const DatasetName& dataset_name,
           const NumericTableType numeric_table_type,
           const KernelType kernel_type)
        : CommonAlgorithmParams(dataset_name, numeric_table_type),
          kernel_type(kernel_type) {}

    const KernelType kernel_type;
  };

  GetterParamsSvmPredict(const Params& params) : params(params) {}

  static DictionaryParams<Params> get_params() {
    return {
      { "Higgs:Linear:100k",
        Params(DatasetName("higgs_small_classifier"),
               TableType(SyclHomogen, FPType),
               KernelType(KernelType::linear)) },
      { "Higgs:RBF:100k",
        Params(DatasetName("higgs_small_classifier"),
               TableType(SyclHomogen, FPType),
               KernelType(KernelType::rbf)) },
      { "Epsilon:Linear:50k;",
        Params(DatasetName("epsilion_50k_classifier"),
               TableType(SyclHomogen, FPType),
               KernelType(KernelType::linear)) },
      { "Epsilon:Linear:50k;",
        Params(DatasetName("epsilion_50k_classifier"),
               TableType(SyclHomogen, FPType),
               KernelType(KernelType::rbf)) },
    };
  }

  Params params;
};

} // namespace dalbench

#endif // DALBENCH_PREDICT_SVM_PARAMS_HPP_
