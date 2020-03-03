/** file gbt_regression.cpp */
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

namespace dalbench {
namespace gbt_regression {

namespace daal_gbt_reg_train      = daal::algorithms::gbt::regression::training;
namespace daal_gbt_reg_prediction = daal::algorithms::gbt::regression::prediction;

template <typename DeviceType, typename FPType>
class GBTReg : public FixtureBatch<daal_gbt_reg_train::Batch<FPType>, DeviceType> {
public:
  using AlgorithmType      = typename daal_gbt_reg_train::Batch<FPType>;
  using AlgorithmParamType = typename AlgorithmType::ParameterType;
  using maxBins            = size_t;
  using maxIterations      = size_t;
  using maxTreeDepth       = size_t;

  struct GBTParams : public CommonAlgorithmParams {
    GBTParams(const DatasetName& dataset_name,
              const NumericTableType numeric_table_type,
              const size_t maxBins,
              const size_t maxIterations,
              const size_t maxTreeDepth)
        : CommonAlgorithmParams(dataset_name, numeric_table_type),
          maxBins(maxBins),
          maxIterations(maxIterations),
          maxTreeDepth(maxTreeDepth) {}

    size_t maxBins;
    size_t maxIterations;
    size_t maxTreeDepth;
  };

  GBTReg(const std::string& name, const GBTParams& params)
      : params_(params),
        FixtureBatch<AlgorithmType, DeviceType>(name, params_) {}

  static DictionaryParams<GBTParams> get_params() {
    return { { "YearMsdTrain",
               GBTParams(DatasetName("year_prediction_msd"),
                         TableType(SyclHomogen, FPType),
                         maxBins(256),
                         maxIterations(20),
                         maxTreeDepth(6)) },
             { "Higgs:1M",
               GBTParams(DatasetName("higgs_1M_reg"),
                         TableType(SyclHomogen, FPType),
                         maxBins(256),
                         maxIterations(20),
                         maxTreeDepth(6)) } };
  }

protected:
  void set_algorithm() final {
    this->algorithm_ = std::make_unique<AlgorithmType>(AlgorithmType());
  }

  void set_input(benchmark::State& state) final {
    auto X = params_.dataset.train().x();
    auto Y = params_.dataset.train().y();

    this->algorithm_->input.set(daal_gbt_reg_train::data, X);
    this->algorithm_->input.set(daal_gbt_reg_train::dependentVariable, Y);
  }

  void set_parameters() final {
    this->algorithm_->parameter().maxBins       = params_.maxBins;
    this->algorithm_->parameter().maxIterations = params_.maxIterations;
    this->algorithm_->parameter().maxTreeDepth  = params_.maxTreeDepth;
  }

  MetricParams check_result(benchmark::State& state) final {
    auto x = params_.dataset.test().x();
    auto y = params_.dataset.test().y();

    daal_gbt_reg_prediction::Batch<FPType> predict_algorithm;

    auto train_model = this->algorithm_->getResult()->get(daal_gbt_reg_train::model);
    predict_algorithm.input.set(daal_gbt_reg_prediction::data, x);
    predict_algorithm.input.set(daal_gbt_reg_prediction::model, train_model);

    predict_algorithm.compute();

    daal_gbt_reg_prediction::ResultPtr prediction_result = predict_algorithm.getResult();
    auto y_predict = prediction_result->get(daal_gbt_reg_prediction::prediction);

    return Metric<MetricType::Mse>::compute_metric(y, y_predict);
  }

private:
  GBTParams params_;
};

DAL_BENCH_REGISTER(GBTReg, CpuDevice, float);
DAL_BENCH_REGISTER(GBTReg, CpuDevice, double);

#if defined(DPCPP_INTERFACES) && (__INTEL_DAAL_BUILD_DATE >= ONEDAL_VERSION_2021_BETA_03_UPDATE)
// FAILED FOR BETA05
// DAL_BENCH_REGISTER(GBTReg, GpuDevice, float);
// DAL_BENCH_REGISTER(GBTReg, GpuDevice, double);
#endif

} // namespace gbt_regression
} // namespace dalbench
