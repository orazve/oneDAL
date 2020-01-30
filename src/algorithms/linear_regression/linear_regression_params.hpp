/** file linear_regression_params.hpp */
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

#ifndef DALBENCH_LINEAR_REGRESSION_PARAMS_HPP_
#define DALBENCH_LINEAR_REGRESSION_PARAMS_HPP_

#include "fixture.hpp"

namespace dalbench {

template <typename FPType>
class GetterParamsLinRegTrain {
public:
  using InterceptFlag = bool;

  struct Params : public CommonAlgorithmParams {
    Params(const DatasetName& dataset_name,
           const NumericTableType numeric_table_type,
           const bool intercept_flag)
        : CommonAlgorithmParams(dataset_name, numeric_table_type),
          intercept_flag(intercept_flag),
          normalize_flag(false) {}

    const bool intercept_flag;
    const bool normalize_flag;
  };

  GetterParamsLinRegTrain(const Params& params) : params(params) {}

  static DictionaryParams<Params> get_params() {
    return {
      { "YearMsdTrain:463K",
        Params(DatasetName("year_prediction_msd"),
               TableType(SyclHomogen, FPType),
               InterceptFlag(true)) },
      { "Higgs:1M",
        Params(DatasetName("higgs_1M_reg"), TableType(SyclHomogen, FPType), InterceptFlag(true)) }
    };
  }

  Params params;
};

} // namespace dalbench

#endif // DALBENCH_LINEAR_REGRESSION_PARAMS_HPP_
