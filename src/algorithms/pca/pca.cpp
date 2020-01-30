/** file pca.cpp */
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
#include "pca_params.hpp"

namespace dalbench {
namespace pca {

template <typename DeviceType, typename FPType>
class PcaCov : public GetterParamsPcaCov<FPType>,
               public FixtureBatch<daal_pca::Batch<FPType>, DeviceType> {
public:
  using AlgorithmType = typename daal_pca::Batch<FPType>;
  using GetterParamsPcaCov<FPType>::params;

  PcaCov(const std::string& name, const typename GetterParamsPcaCov<FPType>::Params& params_in)
      : GetterParamsPcaCov<FPType>(params_in),
        FixtureBatch<AlgorithmType, DeviceType>(name, params) {}

protected:
  void set_algorithm() final {
    this->algorithm_ = std::make_unique<AlgorithmType>(AlgorithmType());
  }

  void set_input(benchmark::State& state) final {
    params.n_components = int(params.num_features * params.ratio_components);

    auto x = params.dataset.full().x();
    this->algorithm_->input.set(daal_pca::data, x);
  }

  void set_parameters() final {
    this->algorithm_->parameter.isDeterministic  = true;
    this->algorithm_->parameter.nComponents      = params.n_components;
    this->algorithm_->parameter.resultsToCompute = params.compute_id;
  }
};

DAL_BENCH_REGISTER(PcaCov, CpuDevice, float);
DAL_BENCH_REGISTER(PcaCov, CpuDevice, double);

#ifdef DPCPP_INTERFACES
DAL_BENCH_REGISTER(PcaCov, GpuDevice, float);
DAL_BENCH_REGISTER(PcaCov, GpuDevice, double);
#endif

} // namespace pca
} // namespace dalbench
