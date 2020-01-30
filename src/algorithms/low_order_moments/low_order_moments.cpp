/** file low_order_moments.cpp */
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

#include "low_order_moments_params.hpp"

namespace dalbench {
namespace low_order_moments {

namespace daal_low_order_moments = daal::algorithms::low_order_moments;

template <typename DeviceType, typename FPType>
class Moments : public GetterParamsMoments<FPType>,
                public FixtureBatch<daal_low_order_moments::Batch<FPType>, DeviceType> {
public:
  using AlgorithmType = typename daal_low_order_moments::Batch<FPType>;
  using GetterParamsMoments<FPType>::params;

  Moments(const std::string& name, const typename GetterParamsMoments<FPType>::Params& params_in)
      : GetterParamsMoments<FPType>(params_in),
        FixtureBatch<AlgorithmType, DeviceType>(name, params) {}

protected:
  void set_algorithm() final {
    this->algorithm_ = std::make_unique<AlgorithmType>(AlgorithmType());
  }

  void set_parameters() final {
    this->algorithm_->parameter.estimatesToCompute = daal_low_order_moments::estimatesMeanVariance;
  }

  void set_input(benchmark::State& state) final {
    auto x = params.dataset.full().x();
    this->algorithm_->input.set(daal_low_order_moments::data, x);
  }
};

DAL_BENCH_REGISTER(Moments, CpuDevice, float);
DAL_BENCH_REGISTER(Moments, CpuDevice, double);

#if defined(DPCPP_INTERFACES) && (__INTEL_DAAL_BUILD_DATE >= ONEDAL_VERSION_2021_BETA_03_UPDATE)
DAL_BENCH_REGISTER(Moments, GpuDevice, float);
DAL_BENCH_REGISTER(Moments, GpuDevice, double);
#endif // defined(DPCPP_INTERFACES) && (__INTEL_DAAL_BUILD_DATE >= ONEDAL_VERSION_2021_BETA_03_UPDATE)

} // namespace low_order_moments
} // namespace dalbench
