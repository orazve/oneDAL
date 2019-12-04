/** file low_order_moments_online.cpp */
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

const size_t higgs_blocks   = 4;
const size_t epsilon_blocks = 4;

template <typename DeviceType, typename FPType>
class LowOrderMomentsOnline :
  public FixtureOnline<daal_low_order_moments::Online<FPType>, DeviceType> {
public:
  using AlgorithmType = typename daal_low_order_moments::Online<FPType>;

  struct LowOrderMomentsParams : public CommonAlgorithmParams {
    LowOrderMomentsParams(const DatasetName& dataset_name,
                          const NumericTableType numeric_table_type,
                          const size_t num_blocks) :
      CommonAlgorithmParams(dataset_name, numeric_table_type, num_blocks) {
    }
  };

  LowOrderMomentsOnline(const std::string& name, const LowOrderMomentsParams& params) :
    params_(params), FixtureOnline<AlgorithmType, DeviceType>(params_) {
    this->SetName(name.c_str());
  }

  static DictionaryParams<LowOrderMomentsParams> get_params() {
    return { { "Higgs:1M",
               LowOrderMomentsParams(DatasetName("higgs_1M"),
                                     TableType(SyclHomogen, FPType),
                                     higgs_blocks) },
             { "Epsilon:30K",
               LowOrderMomentsParams(DatasetName("epsilon_30k"),
                                     TableType(SyclHomogen, FPType),
                                     epsilon_blocks) } };
  }

protected:
  void set_algorithm() final {
    this->algorithm_ = std::make_unique<AlgorithmType>(AlgorithmType());
  }

  void set_input_block(const size_t block_index) final {
    auto x_block = params_.dataset.full().x_block(block_index);
    this->algorithm_->input.set(daal_low_order_moments::data, x_block);
  }

private:
  LowOrderMomentsParams params_;
};

DAAL_BENCH_REGISTER(LowOrderMomentsOnline, CpuDevice, float);
DAAL_BENCH_REGISTER(LowOrderMomentsOnline, CpuDevice, double);

#if INTEL_DAAL_VERSION >= ONEDAL_VERSION_2021_U1_BETA_04
  #ifdef DPCPP_INTERFACES
DAAL_BENCH_REGISTER(LowOrderMomentsOnline, GpuDevice, float);
DAAL_BENCH_REGISTER(LowOrderMomentsOnline, GpuDevice, double);
  #endif // DPCPP_INTERFACES
#endif // INTEL_DAAL_VERSION >= ONEDAL_VERSION_2021_U1_BETA_04

} // end namespace low_order_moments
} // end namespace dalbench
