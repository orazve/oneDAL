/** file low_order_moments_batch.cpp */
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
namespace low_order_moments {

namespace daal_low_order_moments = daal::algorithms::low_order_moments;

template <typename DeviceType, typename FPType>
class LowOrderMomentsBatch :
  public FixtureBatch<daal_low_order_moments::Batch<FPType>, DeviceType> {
public:
  using AlgorithmType = typename daal_low_order_moments::Batch<FPType>;

  struct LowOrderMomentsParams : public CommonAlgorithmParams {
    LowOrderMomentsParams(const DatasetName& dataset_name,
                          const NumericTableType numeric_table_type) :
      CommonAlgorithmParams(dataset_name, numeric_table_type) {
    }
  };

  LowOrderMomentsBatch(const std::string& name, const LowOrderMomentsParams& params) :
    params_(params), FixtureBatch<AlgorithmType, DeviceType>(params_) {
    this->SetName(name.c_str());
  }

  static DictionaryParams<LowOrderMomentsParams> get_params() {
    return { { "Higgs:2M",
               LowOrderMomentsParams(DatasetName("higgs_2M"), TableType(SyclHomogen, FPType)) },
             { "Epsilon:80K",
               LowOrderMomentsParams(DatasetName("epsilon_80k"),
                                     TableType(SyclHomogen, FPType)) } };
  }

protected:
  void set_algorithm() final {
    this->algorithm_ = std::make_unique<AlgorithmType>(AlgorithmType());
  }

  void set_input() final {
    auto x = params_.dataset.full().x();
    this->algorithm_->input.set(daal_low_order_moments::data, x);
  }

private:
  LowOrderMomentsParams params_;
};

DAAL_BENCH_REGISTER(LowOrderMomentsBatch, CpuDevice, float);
DAAL_BENCH_REGISTER(LowOrderMomentsBatch, CpuDevice, double);

#if INTEL_DAAL_VERSION >= ONEDAL_VERSION_2021_U1_BETA_04
  #ifdef DPCPP_INTERFACES
DAAL_BENCH_REGISTER(LowOrderMomentsBatch, GpuDevice, float);
DAAL_BENCH_REGISTER(LowOrderMomentsBatch, GpuDevice, double);
  #endif // DPCPP_INTERFACES
#endif // INTEL_DAAL_VERSION >= ONEDAL_VERSION_2021_U1_BETA_04

} // end namespace low_order_moments
} // end namespace dalbench
