/** file linear_regression_predict.cpp */
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

#include "linear_regression_predict_params.hpp"

namespace dalbench {
namespace lin_reg {

namespace daal_lin_reg_train   = daal::algorithms::linear_regression::training;
namespace daal_lin_reg_predict = daal::algorithms::linear_regression::prediction;

template <typename DeviceType, typename FPType>
class LinRegPredict : public GetterParamsLinRegPredict<FPType>,
                      public FixtureBatch<daal_lin_reg_predict::Batch<FPType>, DeviceType> {
public:
  using GetterParamsLinRegPredict<FPType>::params;

  using AlgorithmType = typename daal_lin_reg_predict::Batch<FPType>;

  LinRegPredict(const std::string& name,
                const typename GetterParamsLinRegPredict<FPType>::Params& params_in)
      : GetterParamsLinRegPredict<FPType>(params_in),
        FixtureBatch<AlgorithmType, DeviceType>(name, params) {}

protected:
  void set_algorithm() final {
    this->algorithm_ = std::make_unique<AlgorithmType>(daal_lin_reg_predict::Batch<FPType>());
  }

  void set_input(benchmark::State& state) final {
    auto x = params.dataset.train().x();
    auto y = params.dataset.train().y();

    daal_lin_reg_train::Batch<FPType> lr_algorithm;

    lr_algorithm.input.set(daal_lin_reg_train::data, x);
    lr_algorithm.input.set(daal_lin_reg_train::dependentVariables, y);

    lr_algorithm.compute();
    auto result_model = lr_algorithm.getResult()->get(daal_lin_reg_train::model);

    this->algorithm_->input.set(daal_lin_reg_predict::data, x);
    this->algorithm_->input.set(daal_lin_reg_predict::model, result_model);
  }

  MetricParams check_result(benchmark::State& state) final {
    auto x = params.dataset.train().x();
    auto y = params.dataset.train().y();

    auto prediction_result = this->algorithm_->getResult();
    auto y_predict         = prediction_result->get(daal_lin_reg_predict::prediction);

    return Metric<MetricType::Mse>::compute_metric(y, y_predict);
  }
};

DAL_BENCH_REGISTER(LinRegPredict, CpuDevice, float);
DAL_BENCH_REGISTER(LinRegPredict, CpuDevice, double);

#if defined(DPCPP_INTERFACES) && (__INTEL_DAAL_BUILD_DATE >= ONEDAL_VERSION_2021_BETA_03_UPDATE)
DAL_BENCH_REGISTER(LinRegPredict, GpuDevice, float);
DAL_BENCH_REGISTER(LinRegPredict, GpuDevice, double);
#endif // defined(DPCPP_INTERFACES) && (__INTEL_DAAL_BUILD_DATE >= ONEDAL_VERSION_2021_BETA_03_UPDATE)

} // namespace lin_reg
} // namespace dalbench
