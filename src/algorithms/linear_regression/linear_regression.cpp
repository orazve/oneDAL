/** file linear_regression.cpp */
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
namespace lin_reg {

namespace daal_lin_reg_train = daal::algorithms::linear_regression::training;

template <typename DeviceType, typename FPType>
class LinRegTrain : public FixtureBatch<daal_lin_reg_train::Batch<FPType>, DeviceType> {
public:
    using AlgorithmType = typename daal_lin_reg_train::Batch<FPType>;
    using InterceptFlag = bool;

    struct LinRegParams : public CommonAlgorithmParams {
        LinRegParams(const DatasetName& dataset_name,
                     const NumericTableType numeric_table_type,
                     const bool intercept_flag)
                : CommonAlgorithmParams(dataset_name, numeric_table_type),
                  intercept_flag(intercept_flag) {}

        const bool intercept_flag;
    };

    using DictionaryAlgParams = DictionaryParams<LinRegParams>;

    LinRegTrain(const std::string& name, const LinRegParams& params)
            : params_(params),
              FixtureBatch<AlgorithmType, DeviceType>(params_) {
        this->SetName(name.c_str());
    }

    static DictionaryParams<LinRegParams> get_params() {
        return { { "YearMsdTrain",
                   LinRegParams(DatasetName("year_prediction_msd"),
                                TableType(SyclHomogen, FPType),
                                InterceptFlag(false)) },
                 { "Higgs:1M",
                   LinRegParams(DatasetName("higgs_1M_reg"),
                                TableType(SyclHomogen, FPType),
                                InterceptFlag(true)) } };
    }

protected:
    void set_algorithm() final {
        this->algorithm_ = std::make_unique<AlgorithmType>(daal_lin_reg_train::Batch<FPType>());
    }

    void set_input() final {
        auto x = params_.dataset.train().x();
        auto y = params_.dataset.train().y();

        this->algorithm_->input.set(daal_lin_reg_train::data, x);
        this->algorithm_->input.set(daal_lin_reg_train::dependentVariables, y);
    }

    void set_parameters() final {
        this->algorithm_->parameter.interceptFlag = params_.intercept_flag;
    }

private:
    LinRegParams params_;
};

DAAL_BENCH_REGISTER(LinRegTrain, CpuDevice, float);
DAAL_BENCH_REGISTER(LinRegTrain, CpuDevice, double);

#if defined(DPCPP_INTERFACES) && (__INTEL_DAAL_BUILD_DATE >= ONEDAL_VERSION_2021_BETA_03_UPDATE)
DAAL_BENCH_REGISTER(LinRegTrain, GpuDevice, float);
DAAL_BENCH_REGISTER(LinRegTrain, GpuDevice, double);
#endif // defined(DPCPP_INTERFACES) && (__INTEL_DAAL_BUILD_DATE >= ONEDAL_VERSION_2021_BETA_03_UPDATE)

} // end namespace lin_reg
} // end namespace dalbench
