/** file df_predict.cpp */
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

#include "df_predict_params.hpp"

namespace dalbench {
namespace df_predict {

namespace daal_df_train         = daal::algorithms::decision_forest::classification::training;
namespace daal_df_predict       = daal::algorithms::decision_forest::classification::prediction;
namespace daal_classifier_train = daal::algorithms::classifier::training;
namespace daal_classifier_prediction = daal::algorithms::classifier::prediction;

template <typename DeviceType, typename FPType>
class ForestPredict : public GetterParamsForestPredict<FPType>,
                      public FixtureBatch<daal_df_predict::Batch<FPType>, DeviceType> {
public:
  using GetterParamsForestPredict<FPType>::params;

  using AlgorithmType = typename daal_df_predict::Batch<FPType>;

  ForestPredict(const std::string& name,
                const typename GetterParamsForestPredict<FPType>::Params& params_in)
      : GetterParamsForestPredict<FPType>(params_in),
        FixtureBatch<AlgorithmType, DeviceType>(name, params) {}

protected:
  void set_algorithm() final {
    this->algorithm_ = std::make_unique<AlgorithmType>(AlgorithmType(params.num_responses));
  }

  void set_input(benchmark::State& state) final {
    auto x = params.dataset.train().x();
    auto y = params.dataset.train().y();

    daal_df_train::Batch<FPType> train_algorithm(params.num_responses);

    train_algorithm.input.set(daal_classifier_train::data, x);
    train_algorithm.input.set(daal_classifier_train::labels, y);

    train_algorithm.parameter.observationsPerTreeFraction = 1.0;
    train_algorithm.parameter.featuresPerNode             = 0;
    train_algorithm.parameter.seed                        = 777;
    train_algorithm.parameter.nTrees                      = params.num_trees;
    train_algorithm.parameter.maxTreeDepth                = params.max_tree_depth;

    train_algorithm.compute();
    auto result_model = train_algorithm.getResult()->get(daal_classifier_train::model);

    this->algorithm_->input.set(daal_classifier_prediction::data, x);
    this->algorithm_->input.set(daal_classifier_prediction::model, result_model);
  }

  MetricParams check_result(benchmark::State& state) final {
    auto x = params.dataset.train().x();
    auto y = params.dataset.train().y();

    auto prediction_result = this->algorithm_->getResult();
    auto y_predict         = prediction_result->get(daal_classifier_prediction::prediction);

    return Metric<MetricType::Accuracy>::compute_metric(y, y_predict);
  }
};

DAL_BENCH_REGISTER(ForestPredict, CpuDevice, float);
DAL_BENCH_REGISTER(ForestPredict, CpuDevice, double);

} // namespace df_predict
} // namespace dalbench
