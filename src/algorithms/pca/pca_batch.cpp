/** file pca_batch.cpp */
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
namespace pca {

namespace daal_pca = daal::algorithms::pca;

template <typename DeviceType, typename FPType>
class PCABatch : public FixtureBatch<daal_pca::Batch<FPType>, DeviceType> {
public:
  using AlgorithmType = typename daal_pca::Batch<FPType>;
  using nComponent    = FPType;

  struct PcaParams : public CommonAlgorithmParams {
    PcaParams(const DatasetName& dataset_name,
              const NumericTableType numeric_table_type,
              const daal_pca::ResultToComputeId compute_id,
              const FPType ratio_components) :
      CommonAlgorithmParams(dataset_name, numeric_table_type),
      compute_id(compute_id), ratio_components(ratio_components) {
    }

    const daal_pca::ResultToComputeId compute_id;
    const FPType ratio_components;
  };

  PCABatch(const std::string& name, const PcaParams& params) :
    params_(params), FixtureBatch<AlgorithmType, DeviceType>(params_) {
    this->SetName(name.c_str());
  }

  static DictionaryParams<PcaParams> get_params() {
    return { { "Higgs:1M",
               PcaParams(DatasetName("higgs_1M"),
                         TableType(SyclHomogen, FPType),
                         daal_pca::ResultToComputeId(daal_pca::eigenvalues |
                                                     daal_pca::eigenvectors | daal_pca::variances),
                         nComponent(0.6)) },
             { "Epsilon:30K",
               PcaParams(DatasetName("epsilon_30k"),
                         TableType(SyclHomogen, FPType),
                         daal_pca::ResultToComputeId(daal_pca::eigenvalues |
                                                     daal_pca::eigenvectors | daal_pca::variances),
                         nComponent(0.6)) } };
  }

protected:
  void set_algorithm() final {
    this->algorithm_ = std::make_unique<AlgorithmType>(AlgorithmType());
  }

  void set_input() final {
    auto x               = params_.dataset.full().x();
    params_.num_features = x->getNumberOfColumns();
    this->algorithm_->input.set(daal_pca::data, x);
  }

  void set_parameters() final {
    this->algorithm_->parameter.isDeterministic = true;
    this->algorithm_->parameter.nComponents =
      size_t(params_.num_features * params_.ratio_components);
    this->algorithm_->parameter.resultsToCompute = params_.compute_id;
  }

private:
  PcaParams params_;
};

DAAL_BENCH_REGISTER(PCABatch, CpuDevice, float);
DAAL_BENCH_REGISTER(PCABatch, CpuDevice, double);

#ifdef DPCPP_INTERFACES
DAAL_BENCH_REGISTER(PCABatch, GpuDevice, float);
DAAL_BENCH_REGISTER(PCABatch, GpuDevice, double);
#endif

} // end namespace pca
} // end namespace dalbench
