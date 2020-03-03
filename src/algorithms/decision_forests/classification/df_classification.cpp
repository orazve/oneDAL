/** file df_classification.cpp */
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

#include "df_classification_params.hpp"

namespace dalbench {
namespace df_classification {

namespace daal_df_classification_train =
  daal::algorithms::decision_forest::classification::training;
namespace daal_df_classification_pred =
  daal::algorithms::decision_forest::classification::prediction;
namespace daal_classifier_train      = daal::algorithms::classifier::training;
namespace daal_classifier_prediction = daal::algorithms::classifier::prediction;
namespace daal_classifier            = daal::algorithms::classifier;

template <typename DeviceType, typename FPType>
class ForestCls : public GetterParamsForestCls<FPType>,
                  public FixtureBatch<daal_df_classification_train::Batch<FPType>, DeviceType> {
public:
  using GetterParamsForestCls<FPType>::params;

  using AlgorithmType = typename daal_df_classification_train::Batch<FPType>;

  ForestCls(const std::string& name,
            const typename GetterParamsForestCls<FPType>::Params& params_in)
      : GetterParamsForestCls<FPType>(params_in),
        FixtureBatch<AlgorithmType, DeviceType>(name, params) {}

protected:
  void set_algorithm() final {
    this->algorithm_ = std::make_unique<AlgorithmType>(AlgorithmType(params.num_responses));
  }

  void set_input(benchmark::State& state) final {
    auto x = params.dataset.train().x();
    auto y = params.dataset.train().y();

    this->algorithm_->input.set(daal_classifier_train::data, x);
    this->algorithm_->input.set(daal_classifier_train::labels, y);
  }

  void set_parameters() final {
    this->algorithm_->parameter.observationsPerTreeFraction = 1.0;
    this->algorithm_->parameter.featuresPerNode             = 0;
    this->algorithm_->parameter.seed                        = 777;
    this->algorithm_->parameter.nTrees                      = params.num_trees;
    this->algorithm_->parameter.maxTreeDepth                = params.max_tree_depth;
  }

  MetricParams check_result(benchmark::State& state) final {
    auto x = params.dataset.test().x();
    auto y = params.dataset.test().y();

    daal_df_classification_pred::Batch<FPType> predict_algorithm(params.num_responses);

    auto train_model = this->algorithm_->getResult()->get(daal_classifier_train::model);
    predict_algorithm.input.set(daal_classifier_prediction::data, x);
    predict_algorithm.input.set(daal_classifier_prediction::model, train_model);

    predict_algorithm.compute();

    auto prediction_result = predict_algorithm.getResult();
    auto y_predict         = prediction_result->get(daal_classifier::prediction::prediction);

    return Metric<MetricType::Accuracy>::compute_metric(y, y_predict);
  }
};

DAL_BENCH_REGISTER(ForestCls, CpuDevice, float);
DAL_BENCH_REGISTER(ForestCls, CpuDevice, double);

} // namespace df_classification
} // namespace dalbench
