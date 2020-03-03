/** file pca_transform_batch.cpp */
/*******************************************************************************
* Copyright 2020 Intel Corporation
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
#include "pca_transform_params.hpp"

namespace dalbench {
namespace pca_transform {

template <typename DeviceType, typename FPType>
class PcaTransform : public GetterParamsPcaTransform<FPType>,
                     public FixtureBatch<daal_pca_transform::Batch<FPType>, DeviceType> {
public:
  using AlgorithmType = typename daal_pca_transform::Batch<FPType>;
  using GetterParamsPcaTransform<FPType>::params;

  PcaTransform(const std::string& name,
               const typename GetterParamsPcaTransform<FPType>::Params& params_in)
      : GetterParamsPcaTransform<FPType>(params_in),
        FixtureBatch<AlgorithmType, DeviceType>(name, params) {}

protected:
  void set_algorithm() final {
    this->algorithm_ = std::make_unique<AlgorithmType>(AlgorithmType());
  }

  void set_input(benchmark::State& state) final {
    auto x              = params.dataset.full().x();
    params.num_features = x->getNumberOfColumns();
    daal_pca::Batch<FPType> pca_algorithm;
    pca_algorithm.input.set(daal_pca::data, x);
    pca_algorithm.parameter.resultsToCompute =
      daal_pca::mean | daal_pca::variance | daal_pca::eigenvalue;
    pca_algorithm.parameter.isDeterministic = true;
    pca_algorithm.compute();
    auto result = pca_algorithm.getResult();
    this->algorithm_->input.set(daal_pca_transform::data, x);
    this->algorithm_->input.set(daal_pca_transform::eigenvectors,
                                result->get(daal_pca::eigenvectors));
    this->algorithm_->input.set(daal_pca_transform::dataForTransform,
                                result->get(daal_pca::dataForTransform));
  }

  void set_parameters() final {
    this->algorithm_->parameter.nComponents = size_t(params.num_features * params.ratio_components);
  }
};

DAL_BENCH_REGISTER(PcaTransform, CpuDevice, float);
DAL_BENCH_REGISTER(PcaTransform, CpuDevice, double);

#if defined(DPCPP_INTERFACES) && (__INTEL_DAAL_BUILD_DATE >= ONEDAL_VERSION_2021_BETA_04)
DAL_BENCH_REGISTER(PcaTransform, GpuDevice, float);
DAL_BENCH_REGISTER(PcaTransform, GpuDevice, double);
#endif // defined(DPCPP_INTERFACES) && (__INTEL_DAAL_BUILD_DATE >= ONEDAL_VERSION_2021_BETA_04)

} // namespace pca_transform
} // namespace dalbench
