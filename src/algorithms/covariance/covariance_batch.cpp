/** file covariance_batch.cpp */
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
namespace covariance {

namespace daal_covariance = daal::algorithms::covariance;

template <typename DeviceType, typename FPType>
class CovarianceBatch : public FixtureBatch<daal_covariance::Batch<FPType>, DeviceType> {
public:
  using AlgorithmType = typename daal_covariance::Batch<FPType>;

  struct CovarianceParams : public CommonAlgorithmParams {
    CovarianceParams(const DatasetName& dataset_name, const NumericTableType numeric_table_type)
        : CommonAlgorithmParams(dataset_name, numeric_table_type) {}
  };

  CovarianceBatch(const std::string& name, const CovarianceParams& params)
      : params_(params),
        FixtureBatch<AlgorithmType, DeviceType>(params_) {
    this->SetName(name.c_str());
  }

  static DictionaryParams<CovarianceParams> get_params() {
    return { { "Higgs:1M",
               CovarianceParams(DatasetName("higgs_1M"), TableType(SyclHomogen, FPType)) },
             { "Epsilon:30K",
               CovarianceParams(DatasetName("epsilon_30k"), TableType(SyclHomogen, FPType)) } };
  }

protected:
  void set_algorithm() final {
    this->algorithm_ = std::make_unique<AlgorithmType>(AlgorithmType());
  }

  void set_input() final {
    auto x = params_.dataset.full().x();
    this->algorithm_->input.set(daal_covariance::data, x);
  }

private:
  CovarianceParams params_;
};

DAAL_BENCH_REGISTER(CovarianceBatch, CpuDevice, float);
DAAL_BENCH_REGISTER(CovarianceBatch, CpuDevice, double);

#ifdef DPCPP_INTERFACES
DAAL_BENCH_REGISTER(CovarianceBatch, GpuDevice, float);
DAAL_BENCH_REGISTER(CovarianceBatch, GpuDevice, double);
#endif // DPCPP_INTERFACES

} // end namespace covariance
} // end namespace dalbench
