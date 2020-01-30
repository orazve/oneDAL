/** file bf_knn.cpp */
/*******************************************************************************
* Copyright 2019-2020 Intel Corporation
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

#include "bf_knn_params.hpp"

#if __INTEL_DAAL_BUILD_DATE >= ONEDAL_VERSION_2021_BETA_03_UPDATE

namespace dalbench {
namespace bf_knn {

namespace daal_bf_knn_train       = daal::algorithms::bf_knn_classification::training;
namespace daal_bf_knn_predict     = daal::algorithms::bf_knn_classification::prediction;
namespace daal_classifier_train   = daal::algorithms::classifier::training;
namespace daal_classifier_predict = daal::algorithms::classifier::prediction;

template <typename DeviceType, typename FPType>
class KnnBF : public GetterParamsKnnBF<FPType>,
              public FixtureBatch<daal_bf_knn_predict::Batch<FPType>, DeviceType> {
public:
  using AlgorithmType = typename daal_bf_knn_predict::Batch<FPType>;
  using GetterParamsKnnBF<FPType>::params;

  KnnBF(const std::string& name, const typename GetterParamsKnnBF<FPType>::Params& params_in)
      : GetterParamsKnnBF<FPType>(params_in),
        FixtureBatch<AlgorithmType, DeviceType>(name, params) {}

protected:
  void set_algorithm() final {
    this->algorithm_ = std::make_unique<AlgorithmType>(AlgorithmType());
  }

  void set_input(benchmark::State& state) final {
    auto x1 = params.dataset.train().x();
    auto y1 = params.dataset.train().y();

    auto x2 = params.dataset.test().x();

    daal_bf_knn_train::Batch<FPType> train_algo;

    train_algo.getInput()->set(daal_classifier_train::data, x1);
    train_algo.getInput()->set(daal_classifier_train::labels, y1);
    train_algo.compute();
    auto result = train_algo.getResult();
    this->algorithm_->getInput()->set(daal_classifier_predict::data, x2);
    this->algorithm_->getInput()->set(daal_classifier_predict::model,
                                      result->get(daal_classifier_train::model));
  }

  void set_parameters() final {
    this->algorithm_->parameter().k        = params.num_k;
    this->algorithm_->parameter().nClasses = params.num_responses;
  }

  MetricParams check_result(benchmark::State& state) final {
    auto prediction_result = this->algorithm_->getResult();
    auto y_predict         = prediction_result->get(daal_classifier_predict::prediction);

    auto y = params.dataset.test().y();

    return Metric<MetricType::Accuracy>::compute_metric(y, y_predict);
  }
};

  #ifdef DPCPP_INTERFACES
DAL_BENCH_REGISTER(KnnBF, GpuDevice, float);
DAL_BENCH_REGISTER(KnnBF, GpuDevice, double);
  #endif

} // namespace bf_knn
} // namespace dalbench

#endif // INTEL_DAAL_VERSION >= ONEDAL_VERSION_2021_BETA_03_UPDATE
