/** file logistic_regression_sgd_params.hpp */
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

#ifndef DALBENCH_LOGREG_SGD_PARAMS_HPP_
#define DALBENCH_LOGREG_SGD_PARAMS_HPP_

#include "fixture.hpp"

namespace dalbench {

template <typename FPType>
class GetterParamsLogRegTrainSGD {
public:
  using nIterations   = size_t;
  using InterceptFlag = bool;
  using PenaltyL2     = FPType;
  using LearningRate  = FPType;
  using nBatch        = FPType;

  struct Params : public CommonAlgorithmParams {
    Params(const DatasetName& dataset_name,
           const NumericTableType numeric_table_type,
           const size_t num_iterations,
           const bool intercept_flag,
           const FPType penalty_l2,
           const FPType ratio_batch,
           const FPType learning_rate)
        : CommonAlgorithmParams(dataset_name, numeric_table_type),
          num_iterations(num_iterations),
          intercept_flag(intercept_flag),
          penalty_l2(penalty_l2),
          ratio_batch(ratio_batch),
          learning_rate(learning_rate),
          accuracy_threshold(0.0) {}

    const size_t num_iterations;
    const bool intercept_flag;
    const FPType penalty_l2;
    const FPType ratio_batch;
    const FPType learning_rate;
    const FPType accuracy_threshold;
  };

  GetterParamsLogRegTrainSGD(const Params& params) : params(params) {}

  static DictionaryParams<Params> get_params() {
    return { { "Susy:4.5M;Iter:20",
               Params(DatasetName("SUSY"),
                      TableType(SyclHomogen, FPType),
                      nIterations(20),
                      InterceptFlag(true),
                      PenaltyL2(0.0),
                      nBatch(1.0),
                      LearningRate(2.5)) },
             { "Susy:4.5M;Batch:8192;Iter:10",
               Params(DatasetName("SUSY"),
                      TableType(SyclHomogen, FPType),
                      nIterations(10),
                      InterceptFlag(true),
                      PenaltyL2(0.0),
                      nBatch(8192),
                      LearningRate(0.01)) },
             { "Mnist:60K;Iter:50",
               Params(DatasetName("mnist"),
                      TableType(SyclHomogen, FPType),
                      nIterations(50u),
                      InterceptFlag(false),
                      PenaltyL2(0.0),
                      nBatch(1.0),
                      LearningRate(0.5)) },
             { "Mnist:60K;Batch:8192;Iter:500",
               Params(DatasetName("mnist"),
                      TableType(SyclHomogen, FPType),
                      nIterations(500u),
                      InterceptFlag(true),
                      PenaltyL2(0.0),
                      nBatch(8192),
                      LearningRate(1.0)) }

    };
  }

  Params params;
};

} // namespace dalbench

#endif // DALBENCH_LOGREG_PARAMS_HPP_
