/** file logistic_regression_sgd.cpp */
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
#include "logistic_regression_sgd_params.hpp"

namespace dalbench {
namespace log_reg {

namespace daal_log_reg_train         = daal::algorithms::logistic_regression::training;
namespace daal_log_reg_prediction    = daal::algorithms::logistic_regression::prediction;
namespace daal_optimization_solver   = daal::algorithms::optimization_solver;
namespace daal_classifier_train      = daal::algorithms::classifier::training;
namespace daal_classifier_prediction = daal::algorithms::classifier::prediction;
namespace daal_dm                    = daal::data_management;

template <typename DeviceType, typename FPType>
class LogRegSGD : public GetterParamsLogRegSGD<FPType>,
                  public FixtureBatch<daal_log_reg_train::Batch<FPType>, DeviceType> {
public:
  using GetterParamsLogRegSGD<FPType>::params;

  using AlgorithmType = typename daal_log_reg_train::Batch<FPType>;
  using SolverType =
    typename daal_optimization_solver::sgd::Batch<FPType, daal_optimization_solver::sgd::miniBatch>;

  LogRegSGD(const std::string& name,
            const typename GetterParamsLogRegSGD<FPType>::Params& params_in)
      : GetterParamsLogRegSGD<FPType>(params_in),
        FixtureBatch<AlgorithmType, DeviceType>(name, params) {}

protected:
  void set_algorithm() final {
    this->algorithm_ =
      std::make_unique<AlgorithmType>(daal_log_reg_train::Batch<FPType>(params.num_responses));
  }

  void set_input(benchmark::State& state) final {
    auto x = params.dataset.train().x();
    auto y = params.dataset.train().y();

    params.num_observations = x->getNumberOfRows();

    this->algorithm_->input.set(daal_classifier_train::data, x);
    this->algorithm_->input.set(daal_classifier_train::labels, y);
  }

  void set_parameters() final {
    daal::services::SharedPtr<SolverType> solver(new SolverType());

    const size_t batch_size =
      params.ratio_batch > 1.0 ? params.ratio_batch : params.ratio_batch * params.num_observations;
    solver->parameter.innerNIterations = int(params.num_observations / batch_size) + 1;

    solver->parameter.nIterations = params.num_iterations + solver->parameter.innerNIterations;

    solver->parameter.conservativeSequence =
      daal_dm::HomogenNumericTable<FPType>::create(1,
                                                   1,
                                                   daal_dm::NumericTable::doAllocate,
                                                   FPType(0.0));

    solver->parameter.batchSize                      = batch_size;
    solver->parameter.accuracyThreshold              = params.accuracy_threshold;
    this->algorithm_->parameter().optimizationSolver = solver;
    this->algorithm_->parameter().penaltyL2          = params.penalty_l2;
    this->algorithm_->parameter().interceptFlag      = params.intercept_flag;
  }

  MetricParams check_result(benchmark::State& state) final {
    auto x = params.dataset.test().x();
    auto y = params.dataset.test().y();

    daal_log_reg_prediction::Batch<FPType> predict_algorithm(params.num_responses);

    auto train_model = this->algorithm_->getResult()->get(daal_classifier_train::model);
    predict_algorithm.input.set(daal_classifier_prediction::data, x);
    predict_algorithm.input.set(daal_classifier_prediction::model, train_model);

    predict_algorithm.compute();

    daal_log_reg_prediction::ResultPtr prediction_result = predict_algorithm.getResult();
    auto y_predict = prediction_result->get(daal_classifier_prediction::prediction);

    return Metric<MetricType::Accuracy>::compute_metric(y, y_predict);
  }
};

DAL_BENCH_REGISTER(LogRegSGD, CpuDevice, float);
DAL_BENCH_REGISTER(LogRegSGD, CpuDevice, double);

#ifdef DPCPP_INTERFACES
DAL_BENCH_REGISTER(LogRegSGD, GpuDevice, float);
DAL_BENCH_REGISTER(LogRegSGD, GpuDevice, double);
#endif

} // namespace log_reg
} // namespace dalbench
