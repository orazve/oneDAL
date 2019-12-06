/** file covariance_online.cpp */
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
class CovarianceOnline : public FixtureOnline<daal_covariance::Online<FPType>, DeviceType> {
public:
  using AlgorithmType = typename daal_covariance::Online<FPType>;
  using nBlock        = size_t;

  struct CovarianceParams : public CommonAlgorithmParams {
    CovarianceParams(const DatasetName& dataset_name,
                     const NumericTableType numeric_table_type,
                     const size_t num_blocks) :
      CommonAlgorithmParams(dataset_name, numeric_table_type, num_blocks) {
    }
  };

  CovarianceOnline(const std::string& name, const CovarianceParams& params) :
    params_(params), FixtureOnline<AlgorithmType, DeviceType>(params_) {
    this->SetName(name.c_str());
  }

  static DictionaryParams<CovarianceParams> get_params() {
    return {
      { "Higgs:1M",
        CovarianceParams(DatasetName("higgs_1M"), TableType(SyclHomogen, FPType), nBlock(4)) },
      { "Epsilon:30K",
        CovarianceParams(DatasetName("epsilon_30k"), TableType(SyclHomogen, FPType), nBlock(4)) }
    };
  }

protected:
  void set_algorithm() final {
    this->algorithm_ = std::make_unique<AlgorithmType>(AlgorithmType());
  }

  void set_input_block(const size_t block_index) final {
    auto x_block = params_.dataset.full().x_block(block_index);
    this->algorithm_->input.set(daal_covariance::data, x_block);
  }

private:
  CovarianceParams params_;
};

DAAL_BENCH_REGISTER(CovarianceOnline, CpuDevice, float);
DAAL_BENCH_REGISTER(CovarianceOnline, CpuDevice, double);

#if defined(DPCPP_INTERFACES) && (__INTEL_DAAL_BUILD_DATE >= ONEDAL_VERSION_2021_BETA_03_UPDATE)
DAAL_BENCH_REGISTER(CovarianceOnline, GpuDevice, float);
DAAL_BENCH_REGISTER(CovarianceOnline, GpuDevice, double);
#endif // defined(DPCPP_INTERFACES) && (__INTEL_DAAL_BUILD_DATE >= ONEDAL_VERSION_2021_BETA_03_UPDATE)

} // end namespace covariance
} // end namespace dalbench
