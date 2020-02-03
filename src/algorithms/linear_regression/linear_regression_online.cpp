/** file linear_regression_online.cpp */
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

#include "linear_regression_params.hpp"

namespace dalbench {
namespace lin_reg {

namespace daal_lin_reg_train   = daal::algorithms::linear_regression::training;
namespace daal_lin_reg_predict = daal::algorithms::linear_regression::prediction;

template <typename DeviceType, typename FPType>
class LinRegTrainOnline : public GetterParamsLinRegTrain<FPType>,
                          public FixtureOnline<daal_lin_reg_train::Online<FPType>, DeviceType> {
public:
  using GetterParamsLinRegTrain<FPType>::params;

  using AlgorithmType = typename daal_lin_reg_train::Online<FPType>;

  LinRegTrainOnline(const std::string& name,
                    const typename GetterParamsLinRegTrain<FPType>::Params& paramsin)
      : GetterParamsLinRegTrain<FPType>(paramsin),
        FixtureOnline<AlgorithmType, DeviceType>(name, params) {}

protected:
  void set_algorithm() final {
    this->algorithm_ = std::make_unique<AlgorithmType>(daal_lin_reg_train::Online<FPType>());
  }

  void set_input_block(benchmark::State& state, const size_t block_index) final {
    auto x_block = params.dataset.train().x_block(block_index);
    auto y_block = params.dataset.train().y_block(block_index);
    this->algorithm_->input.set(daal_lin_reg_train::data, x_block);
    this->algorithm_->input.set(daal_lin_reg_train::dependentVariables, y_block);
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

DAL_BENCH_REGISTER(LinRegTrainOnline, CpuDevice, float);
DAL_BENCH_REGISTER(LinRegTrainOnline, CpuDevice, double);

#if defined(DPCPP_INTERFACES) && (__INTEL_DAAL_BUILD_DATE >= ONEDAL_VERSION_2021_BETA_05)
DAL_BENCH_REGISTER(LinRegTrainOnline, GpuDevice, float);
DAL_BENCH_REGISTER(LinRegTrainOnline, GpuDevice, double);
#endif // defined(DPCPP_INTERFACES) && (__INTEL_DAAL_BUILD_DATE >= ONEDAL_VERSION_2021_BETA_05)

} // namespace lin_reg
} // namespace dalbench
