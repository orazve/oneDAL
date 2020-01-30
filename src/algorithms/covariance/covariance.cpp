/** file covariance_batch.cpp */
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

#include "covariance_params.hpp"

namespace dalbench {
namespace covariance {

namespace daal_covariance = daal::algorithms::covariance;

template <typename DeviceType, typename FPType>
class Cov : public GetterParamsCov<FPType>,
            public FixtureBatch<daal_covariance::Batch<FPType>, DeviceType> {
public:
  using AlgorithmType = typename daal_covariance::Batch<FPType>;
  using GetterParamsCov<FPType>::params;

  Cov(const std::string& name, const typename GetterParamsCov<FPType>::Params& params_in)
      : GetterParamsCov<FPType>(params_in),
        FixtureBatch<AlgorithmType, DeviceType>(name, params) {}

protected:
  void set_algorithm() final {
    this->algorithm_ = std::make_unique<AlgorithmType>(AlgorithmType());
  }

  void set_input(benchmark::State& state) final {
    auto x = params.dataset.full().x();
    this->algorithm_->input.set(daal_covariance::data, x);
  }
};

DAL_BENCH_REGISTER(Cov, CpuDevice, float);
DAL_BENCH_REGISTER(Cov, CpuDevice, double);

#ifdef DPCPP_INTERFACES
DAL_BENCH_REGISTER(Cov, GpuDevice, float);
DAL_BENCH_REGISTER(Cov, GpuDevice, double);
#endif // DPCPP_INTERFACES

} // namespace covariance
} // namespace dalbench
