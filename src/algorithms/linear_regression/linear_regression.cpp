/** file linear_regression.cpp */
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

#include "fixture.hpp"

#include "linear_regression_params.hpp"

namespace dalbench {
namespace lin_reg {

namespace daal_lin_reg_train   = daal::algorithms::linear_regression::training;
namespace daal_lin_reg_predict = daal::algorithms::linear_regression::prediction;

template <typename DeviceType, typename FPType>
class LinRegTrain : public GetterParamsLinRegTrain<FPType>,
                    public FixtureBatch<daal_lin_reg_train::Batch<FPType>, DeviceType> {
public:
  using GetterParamsLinRegTrain<FPType>::params;

  using AlgorithmType = typename daal_lin_reg_train::Batch<FPType>;

  LinRegTrain(const std::string& name,
              const typename GetterParamsLinRegTrain<FPType>::Params& paramsin)
      : GetterParamsLinRegTrain<FPType>(paramsin),
        FixtureBatch<AlgorithmType, DeviceType>(name, params) {}

protected:
  void set_algorithm() final {
    this->algorithm_ = std::make_unique<AlgorithmType>(daal_lin_reg_train::Batch<FPType>());
  }

  void set_input(benchmark::State& state) final {
    auto x = params.dataset.train().x();
    auto y = params.dataset.train().y();

    this->algorithm_->input.set(daal_lin_reg_train::data, x);
    this->algorithm_->input.set(daal_lin_reg_train::dependentVariables, y);
  }

  void set_parameters() final {
    this->algorithm_->parameter.interceptFlag = params.intercept_flag;
  }

  MetricParams check_result(benchmark::State& state) final {
    auto x = params.dataset.test().x();
    auto y = params.dataset.test().y();

    daal_lin_reg_predict::Batch<FPType> predict_algorithm;

    auto train_model = this->algorithm_->getResult()->get(daal_lin_reg_train::model);
    predict_algorithm.input.set(daal_lin_reg_predict::data, x);
    predict_algorithm.input.set(daal_lin_reg_predict::model, train_model);

    predict_algorithm.compute();

    daal_lin_reg_predict::ResultPtr prediction_result = predict_algorithm.getResult();
    auto y_predict = prediction_result->get(daal_lin_reg_predict::prediction);

    return Metric<MetricType::Mse>::compute_metric(y, y_predict);
  }
};

DAL_BENCH_REGISTER(LinRegTrain, CpuDevice, float);
DAL_BENCH_REGISTER(LinRegTrain, CpuDevice, double);

#if defined(DPCPP_INTERFACES) && (__INTEL_DAAL_BUILD_DATE >= ONEDAL_VERSION_2021_BETA_03_UPDATE)
DAL_BENCH_REGISTER(LinRegTrain, GpuDevice, float);
DAL_BENCH_REGISTER(LinRegTrain, GpuDevice, double);
#endif // defined(DPCPP_INTERFACES) && (__INTEL_DAAL_BUILD_DATE >= ONEDAL_VERSION_2021_BETA_03_UPDATE)

} // namespace lin_reg
} // namespace dalbench
