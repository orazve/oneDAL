/** file covariance_online.cpp */
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

#include "covariance_params.hpp"

namespace dalbench {
namespace covariance {

namespace daal_covariance = daal::algorithms::covariance;

template <typename DeviceType, typename FPType>
class CovOnline : public GetterParamsCov<FPType>,
                  public FixtureOnline<daal_covariance::Online<FPType>, DeviceType> {
public:
  using GetterParamsCov<FPType>::params;
  using AlgorithmType = typename daal_covariance::Online<FPType>;

  CovOnline(const std::string& name, const typename GetterParamsCov<FPType>::Params& params_in)
      : GetterParamsCov<FPType>(params_in),
        FixtureOnline<AlgorithmType, DeviceType>(name, params) {}

protected:
  void set_algorithm() final {
    this->algorithm_ = std::make_unique<AlgorithmType>(AlgorithmType());
  }

  void set_input_block(benchmark::State& state, const size_t block_index) final {
    auto x_block = params.dataset.full().x_block(block_index);
    this->algorithm_->input.set(daal_covariance::data, x_block);
  }
};

DAL_BENCH_REGISTER(CovOnline, CpuDevice, float);
DAL_BENCH_REGISTER(CovOnline, CpuDevice, double);

#if defined(DPCPP_INTERFACES) && (__INTEL_DAAL_BUILD_DATE >= ONEDAL_VERSION_2021_BETA_03_UPDATE)
DAL_BENCH_REGISTER(CovOnline, GpuDevice, float);
DAL_BENCH_REGISTER(CovOnline, GpuDevice, double);
#endif // defined(DPCPP_INTERFACES) && (__INTEL_DAAL_BUILD_DATE >= ONEDAL_VERSION_2021_BETA_03_UPDATE)

} // namespace covariance
} // namespace dalbench
