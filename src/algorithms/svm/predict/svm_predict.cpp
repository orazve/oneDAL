/** file svm_predict.cpp */
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

#include "fixture.hpp"

#include <iostream>
#include <typeinfo>
#include "daal.h"
#include "dataset.hpp"
#include "svm_predict_params.hpp"

namespace dalbench {
namespace svm {

namespace daal_svm_train             = daal::algorithms::svm::training;
namespace daal_svm_predict           = daal::algorithms::svm::prediction;
namespace daal_classifier_train      = daal::algorithms::classifier::training;
namespace daal_classifier_prediction = daal::algorithms::classifier::prediction;
namespace daal_kernel_function       = daal::algorithms::kernel_function;

template <typename DeviceType, typename FPType>
class SvmPredict : public GetterParamsSvmPredict<FPType>,
                   public FixtureBatch<daal_svm_predict::Batch<FPType>, DeviceType> {
public:
  using GetterParamsSvmPredict<FPType>::params;
  using typename GetterParamsSvmPredict<FPType>::KernelType;
  using AlgorithmType = typename daal_svm_predict::Batch<FPType>;

  SvmPredict(const std::string& name,
             const typename GetterParamsSvmPredict<FPType>::Params& params_in)
      : GetterParamsSvmPredict<FPType>(params_in),
        FixtureBatch<AlgorithmType, DeviceType>(name, params) {}

protected:
  void set_algorithm() final {
    this->algorithm_ = std::make_unique<AlgorithmType>(AlgorithmType());
  }

  void set_input(benchmark::State& state) final {
    auto x = params.dataset.train().x();
    auto y = params.dataset.train().y();
    convert_dataset(y);

    daal_kernel_function::KernelIfacePtr kernel_func();
    switch (params.kernel_type) {
      case KernelType::rbf:
        kernel_func =
          daal_kernel_function::KernelIfacePtr(new daal_kernel_function::rbf::Batch<FPType>());
      case KernelType::linear:
        kernel_func =
          daal_kernel_function::KernelIfacePtr(new daal_kernel_function::linear::Batch<FPType>());
    }

    daal_svm_train::Batch<FPType> train_algorithm;
    const size_t num_cache_size                 = std::min(x->getNumberOfRows(), 60000ul);
    train_algorithm.parameter.cacheSize         = num_cache_size * num_cache_size * sizeof(FPType);
    train_algorithm.parameter.accuracyThreshold = 0.1;
    train_algorithm.parameter.C                 = 10000;
    train_algorithm.parameter.maxIterations     = 200;
    train_algorithm.parameter.kernel            = kernel_func;

    train_algorithm.input.set(daal_classifier_train::data, x);
    train_algorithm.input.set(daal_classifier_train::labels, y);

    train_algorithm.compute();

    auto train_model = train_algorithm.getResult()->get(daal_classifier_train::model);

    this->algorithm_->input.set(daal_classifier_prediction::data, x);
    this->algorithm_->input.set(daal_classifier_prediction::model, train_model);
    this->algorithm_->parameter.kernel = kernel_func;
  }

  MetricParams check_result(benchmark::State& state) final {
    auto x                 = params.dataset.train().x();
    auto y                 = params.dataset.train().y();
    auto prediction_result = this->algorithm_->getResult();
    auto y_predict         = prediction_result->get(daal_classifier_prediction::prediction);
    convert_prediction(y_predict);
    return Metric<MetricType::Accuracy>::compute_metric(y, y_predict);
  }

private:
  void convert_dataset(const NumericTablePtr& _dataset) {
    daal::data_management::BlockDescriptor<FPType> svm_block;
    _dataset->getBlockOfRows(0,
                             _dataset->getNumberOfRows(),
                             daal::data_management::readWrite,
                             svm_block);
    FPType* block = svm_block.getBlockPtr();
    for (size_t i = 0; i < _dataset->getNumberOfRows() * _dataset->getNumberOfColumns(); ++i)
      if (block[i] == 0)
        block[i] = -1;
    _dataset->releaseBlockOfRows(svm_block);
  }

  void convert_prediction(const NumericTablePtr& _predict) {
    daal::data_management::BlockDescriptor<FPType> predict_block;
    _predict->getBlockOfRows(0,
                             _predict->getNumberOfRows(),
                             daal::data_management::readWrite,
                             predict_block);
    FPType* block = predict_block.getBlockPtr();
    for (size_t i = 0; i < _predict->getNumberOfRows() * _predict->getNumberOfColumns(); ++i)
      block[i] = block[i] >= 0 ? 1 : -1;
    _predict->releaseBlockOfRows(predict_block);
  }
};

DAL_BENCH_REGISTER(SvmPredict, CpuDevice, float);
DAL_BENCH_REGISTER(SvmPredict, CpuDevice, double);

} // namespace svm
} // namespace dalbench
