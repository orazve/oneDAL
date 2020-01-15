/** file logreg_sgd.cpp */
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
namespace log_reg {

namespace daal_log_reg_train         = daal::algorithms::logistic_regression::training;
namespace daal_log_reg_prediction    = daal::algorithms::logistic_regression::prediction;
namespace daal_optimization_solver   = daal::algorithms::optimization_solver;
namespace daal_classifier            = daal::algorithms::classifier;
namespace daal_classifier_train      = daal::algorithms::classifier::training;
namespace daal_classifier_prediction = daal::algorithms::classifier::prediction;

template <typename DeviceType, typename FPType>
class LogRegTrainSGDBatch : public FixtureBatch<daal_log_reg_train::Batch<FPType>, DeviceType> {
public:
  using AlgorithmType = typename daal_log_reg_train::Batch<FPType>;
  using SolverType =
    typename daal_optimization_solver::sgd::Batch<FPType, daal_optimization_solver::sgd::miniBatch>;
  using nIterations   = size_t;
  using InterceptFlag = bool;
  using PenaltyL2     = FPType;
  using nBatch        = FPType;

  struct LogRegParams : public CommonAlgorithmParams {
    LogRegParams(const DatasetName& dataset_name,
                 const NumericTableType numeric_table_type,
                 const size_t num_iterations,
                 const bool intercept_flag,
                 const FPType penaltyL2,
                 const FPType ratio_batch)
        : CommonAlgorithmParams(dataset_name, numeric_table_type),
          num_iterations(num_iterations),
          intercept_flag(intercept_flag),
          penalty_l2(penalty_l2),
          ratio_batch(ratio_batch),
          accuracy_threshold(0.0) {}

    const size_t num_iterations;
    const bool intercept_flag;
    const FPType penalty_l2;
    const FPType ratio_batch;
    const double accuracy_threshold;
  };

  using DictionaryAlgParams = DictionaryParams<LogRegParams>;

  LogRegTrainSGDBatch(const std::string& name, const LogRegParams& params)
      : params_(params),
        FixtureBatch<AlgorithmType, DeviceType>(params_) {
    this->SetName(name.c_str());
  }

  static DictionaryParams<LogRegParams> get_params() {
    return { { "Susy:4.5M",
               LogRegParams(DatasetName("SUSY"),
                            TableType(SyclHomogen, FPType),
                            nIterations(10),
                            InterceptFlag(false),
                            PenaltyL2(0.0),
                            nBatch(1.0)) },
             { "Susy:4.5M;Batch:8192",
               LogRegParams(DatasetName("SUSY"),
                            TableType(SyclHomogen, FPType),
                            nIterations(200),
                            InterceptFlag(true),
                            PenaltyL2(0.0),
                            nBatch(8192)) },
             { "Mnist:60K",
               LogRegParams(DatasetName("mnist"),
                            TableType(SyclHomogen, FPType),
                            nIterations(100u),
                            InterceptFlag(false),
                            PenaltyL2(1.0),
                            nBatch(1.0)) },
             { "Mnist:60K;Batch:8192",
               LogRegParams(DatasetName("mnist"),
                            TableType(SyclHomogen, FPType),
                            nIterations(500u),
                            InterceptFlag(true),
                            PenaltyL2(1.0),
                            nBatch(8192)) }

    };
  }

protected:
  void set_algorithm() final {
    this->algorithm_ =
      std::make_unique<AlgorithmType>(daal_log_reg_train::Batch<FPType>(params_.num_responses));
  }

  void set_input() final {
    auto x = params_.dataset.train().x();
    auto y = params_.dataset.train().y();

    params_.num_observations = x->getNumberOfRows();

    this->algorithm_->input.set(daal_classifier_train::data, x);
    this->algorithm_->input.set(daal_classifier_train::labels, y);
  }

  void set_parameters() final {
    daal::services::SharedPtr<SolverType> solver(new SolverType());

    solver->parameter.nIterations = params_.num_iterations;

    const size_t batch_size = params_.ratio_batch > 1.0
                                ? params_.ratio_batch
                                : params_.ratio_batch * params_.num_observations;
    solver->parameter.batchSize                      = batch_size;
    solver->parameter.accuracyThreshold              = params_.accuracy_threshold;
    this->algorithm_->parameter().optimizationSolver = solver;
    this->algorithm_->parameter().penaltyL2          = params_.penalty_l2;
    this->algorithm_->parameter().interceptFlag      = params_.intercept_flag;
  }

  MetricParams check_result() final {
    auto x = params_.dataset.test().x();
    auto y = params_.dataset.test().y();

    daal_log_reg_prediction::interface1::Batch<FPType> predict_algorithm(params_.num_responses);
    predict_algorithm.parameter().resultsToCompute |=
      daal_log_reg_prediction::computeClassesProbabilities;

    auto train_model = this->algorithm_->getResult()->get(daal_classifier_train::model);
    predict_algorithm.input.set(daal_classifier_prediction::data, x);
    predict_algorithm.input.set(daal_classifier_prediction::model, train_model);

    predict_algorithm.compute();

    daal_log_reg_prediction::interface1::ResultPtr prediction_result =
      predict_algorithm.getResult();
    auto y_predict = prediction_result->get(daal_classifier::prediction::prediction);

    return Metric<MetricType::Accuracy>::compute_metric(y, y_predict);
  }

private:
  LogRegParams params_;
};

DAAL_BENCH_REGISTER(LogRegTrainSGDBatch, CpuDevice, float);
DAAL_BENCH_REGISTER(LogRegTrainSGDBatch, CpuDevice, double);

#ifdef DPCPP_INTERFACES
DAAL_BENCH_REGISTER(LogRegTrainSGDBatch, GpuDevice, float);
DAAL_BENCH_REGISTER(LogRegTrainSGDBatch, GpuDevice, double);
#endif

} // namespace log_reg
} // namespace dalbench
