/** file logistic_regression_predict.cpp */
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
#include "logistic_regression_predict_params.hpp"

namespace dalbench {
namespace log_reg {

namespace daal_log_reg_train         = daal::algorithms::logistic_regression::training;
namespace daal_log_reg_prediction    = daal::algorithms::logistic_regression::prediction;
namespace daal_optimization_solver   = daal::algorithms::optimization_solver;
namespace daal_classifier_train      = daal::algorithms::classifier::training;
namespace daal_classifier_prediction = daal::algorithms::classifier::prediction;

template <typename DeviceType, typename FPType>
class LogRegPredict : public GetterParamsLogRegPredict<FPType>,
                      public FixtureBatch<daal_log_reg_prediction::Batch<FPType>, DeviceType> {
public:
  using GetterParamsLogRegPredict<FPType>::params;

  using AlgorithmType = typename daal_log_reg_prediction::Batch<FPType>;
  using SolverType =
    typename daal_optimization_solver::sgd::Batch<FPType, daal_optimization_solver::sgd::miniBatch>;

  LogRegPredict(const std::string& name,
                const typename GetterParamsLogRegPredict<FPType>::Params& params_in)
      : GetterParamsLogRegPredict<FPType>(params_in),
        FixtureBatch<AlgorithmType, DeviceType>(name, params) {}

protected:
  void set_algorithm() final {
    this->algorithm_ =
      std::make_unique<AlgorithmType>(daal_log_reg_prediction::Batch<FPType>(params.num_responses));
  }

  void set_input(benchmark::State& state) final {
    auto x = params.dataset.train().x();
    auto y = params.dataset.train().y();

    daal_log_reg_train::Batch<FPType> train_algorithm(params.num_responses);

    daal::services::SharedPtr<SolverType> solver(new SolverType());
    solver->parameter.nIterations                  = 1000;
    solver->parameter.batchSize                    = 512;
    solver->parameter.accuracyThreshold            = 0.001;
    train_algorithm.parameter().optimizationSolver = solver;
    train_algorithm.parameter().penaltyL2          = 0.0;
    train_algorithm.parameter().interceptFlag      = true;

    train_algorithm.input.set(daal_classifier_train::data, x);
    train_algorithm.input.set(daal_classifier_train::labels, y);

    train_algorithm.compute();

    auto train_model = train_algorithm.getResult()->get(daal_classifier_train::model);
    this->algorithm_->input.set(daal_classifier_prediction::data, x);
    this->algorithm_->input.set(daal_classifier_prediction::model, train_model);
  }

  MetricParams check_result(benchmark::State& state) final {
    auto x = params.dataset.train().x();
    auto y = params.dataset.train().y();

    auto prediction_result = this->algorithm_->getResult();
    auto y_predict         = prediction_result->get(daal_classifier_prediction::prediction);

    return Metric<MetricType::Accuracy>::compute_metric(y, y_predict);
  }
};

DAL_BENCH_REGISTER(LogRegPredict, CpuDevice, float);
DAL_BENCH_REGISTER(LogRegPredict, CpuDevice, double);

#ifdef DPCPP_INTERFACES
DAL_BENCH_REGISTER(LogRegPredict, GpuDevice, float);
DAL_BENCH_REGISTER(LogRegPredict, GpuDevice, double);
#endif

} // namespace log_reg
} // namespace dalbench
