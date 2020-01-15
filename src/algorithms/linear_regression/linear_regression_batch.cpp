/** file linear_regression.cpp */
/*******************************************************************************
* Copyright 2019 Intel Corporation
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
namespace lin_reg {

namespace daal_lin_reg_train   = daal::algorithms::linear_regression::training;
namespace daal_lin_reg_predict = daal::algorithms::linear_regression::prediction;

template <typename DeviceType, typename FPType>
class LinRegTrainBatch : public FixtureBatch<daal_lin_reg_train::Batch<FPType>, DeviceType> {
public:
  using AlgorithmType = typename daal_lin_reg_train::Batch<FPType>;
  using InterceptFlag = bool;

  struct LinRegParams : public CommonAlgorithmParams {
    LinRegParams(const DatasetName& dataset_name,
                 const NumericTableType numeric_table_type,
                 const bool intercept_flag)
        : CommonAlgorithmParams(dataset_name, numeric_table_type),
          intercept_flag(intercept_flag) {}

    const bool intercept_flag;
  };

  using DictionaryAlgParams = DictionaryParams<LinRegParams>;

  LinRegTrainBatch(const std::string& name, const LinRegParams& params)
      : params_(params),
        FixtureBatch<AlgorithmType, DeviceType>(params_) {
    this->SetName(name.c_str());
  }

  static DictionaryParams<LinRegParams> get_params() {
    return { { "YearMsdTrain",
               LinRegParams(DatasetName("year_prediction_msd"),
                            TableType(SyclHomogen, FPType),
                            InterceptFlag(false)) },
             { "Higgs:1M",
               LinRegParams(DatasetName("higgs_1M_reg"),
                            TableType(SyclHomogen, FPType),
                            InterceptFlag(true)) } };
  }

protected:
  void set_algorithm() final {
    this->algorithm_ = std::make_unique<AlgorithmType>(daal_lin_reg_train::Batch<FPType>());
  }

  void set_input() final {
    auto x = params_.dataset.train().x();
    auto y = params_.dataset.train().y();

    this->algorithm_->input.set(daal_lin_reg_train::data, x);
    this->algorithm_->input.set(daal_lin_reg_train::dependentVariables, y);
  }

  void set_parameters() final {
    this->algorithm_->parameter.interceptFlag = params_.intercept_flag;
  }

  MetricParams check_result() final {
    auto x = params_.dataset.test().x();
    auto y = params_.dataset.test().y();

    daal_lin_reg_predict::Batch<FPType> predict_algorithm;

    auto train_model = this->algorithm_->getResult()->get(daal_lin_reg_train::model);
    predict_algorithm.input.set(daal_lin_reg_predict::data, x);
    predict_algorithm.input.set(daal_lin_reg_predict::model, train_model);

    predict_algorithm.compute();

    daal_lin_reg_predict::ResultPtr prediction_result = predict_algorithm.getResult();
    auto y_predict = prediction_result->get(daal_lin_reg_predict::prediction);

    return Metric<MetricType::Mse>::compute_metric(y, y_predict);
  }

private:
  LinRegParams params_;
};

DAAL_BENCH_REGISTER(LinRegTrainBatch, CpuDevice, float);
DAAL_BENCH_REGISTER(LinRegTrainBatch, CpuDevice, double);

#if defined(DPCPP_INTERFACES) && (__INTEL_DAAL_BUILD_DATE >= ONEDAL_VERSION_2021_BETA_03_UPDATE)
DAAL_BENCH_REGISTER(LinRegTrainBatch, GpuDevice, float);
DAAL_BENCH_REGISTER(LinRegTrainBatch, GpuDevice, double);
#endif // defined(DPCPP_INTERFACES) && (__INTEL_DAAL_BUILD_DATE >= ONEDAL_VERSION_2021_BETA_03_UPDATE)

} // namespace lin_reg
} // namespace dalbench
