/** file bf_knn_batch.cpp */
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

// New algorithm in oneDAL 2021.1-beta04
#if INTEL_DAAL_VERSION >= ONEDAL_VERSION_2021_U1_BETA_04

namespace dalbench {
namespace bf_knn {

namespace daal_bf_knn_train       = daal::algorithms::bf_knn_classification::training;
namespace daal_bf_knn_predict     = daal::algorithms::bf_knn_classification::prediction;
namespace daal_classifier_train   = daal::algorithms::classifier::training;
namespace daal_classifier_predict = daal::algorithms::classifier::prediction;

template <typename DeviceType, typename FPType>
class KnnPredict : public FixtureBatch<daal_bf_knn_predict::Batch<FPType>, DeviceType> {
public:
  using AlgorithmType = typename daal_bf_knn_predict::Batch<FPType>;
  using kNeighbors    = size_t;

  struct KnnParams : public CommonAlgorithmParams {
    KnnParams(const DatasetName& dataset_name,
              const NumericTableType numeric_type,
              const size_t num_k) :
      CommonAlgorithmParams(dataset_name, numeric_type),
      num_k(num_k) {
    }

    const kNeighbors num_k;
  };

  using DictionaryAlgParams = DictionaryParams<KnnParams>;

  KnnPredict(const std::string& name, const KnnParams& params) :
    params_(params), FixtureBatch<AlgorithmType, DeviceType>(params_) {
    this->SetName(name.c_str());
  }

  static DictionaryParams<KnnParams> get_params() {
    return { { "epsilon",
               KnnParams(DatasetName("epsilion_50k_classifier"),
                         TableType(SyclHomogen, FPType),
                         kNeighbors(5u)) },
             { "higgs",
               KnnParams(DatasetName("higgs_small_classifier"),
                         TableType(SyclHomogen, FPType),
                         kNeighbors(5u)) },
             { "hepmass",
               KnnParams(DatasetName("hepmass_small_classifier"),
                         TableType(SyclHomogen, FPType),
                         kNeighbors(5u)) } };
  }

protected:
  void set_algorithm() final {
    this->algorithm_ = std::make_unique<AlgorithmType>(AlgorithmType());
  }

  void set_input() final {
    auto x1 = params_.dataset.train().x();
    auto y1 = params_.dataset.train().y();

    auto x2 = params_.dataset.test().x();

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
    this->algorithm_->parameter().k = params_.num_k;
  }

private:
  KnnParams params_;
};

  #ifdef DPCPP_INTERFACES
DAAL_BENCH_REGISTER(KnnPredict, GpuDevice, float);
DAAL_BENCH_REGISTER(KnnPredict, GpuDevice, double);
  #endif

} // namespace bf_knn
} // namespace dalbench

#endif // INTEL_DAAL_VERSION >= ONEDAL_VERSION_2021_U1_BETA_04
