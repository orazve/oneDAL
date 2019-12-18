/** file regression.cpp */
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

#include "dataset.hpp"

namespace dalbench {
namespace datasets {

class DATASET(year_prediction_msd) {
    Dataset load(NumericTableType numeric_table_type, size_t num_blocks) override {
        return DatasetFromCsv()
            .path_to_train(
                Workload("year_prediction_msd").path_to_dataset("year_prediction_msd_train.csv"))
            .path_to_test(
                Workload("year_prediction_msd").path_to_dataset("year_prediction_msd_test.csv"))
            .path_to_full(
                Workload("year_prediction_msd").path_to_dataset("year_prediction_msd_full.csv"))
            .num_features(90)
            .num_blocks(num_blocks)
            .regression()
            .on_error(
                "Make sure that year_prediction_msd dataset is downloaded and extracted and DATASETSROOT variable is set correctly.")
            .load(numeric_table_type);
    }
};

class DATASET(higgs_1M_reg) {
    Dataset load(NumericTableType numeric_table_type, size_t num_blocks) override {
        return DatasetFromCsv()
            .path_to_train(Workload("higgs").path_to_dataset("higgs_1m_train.csv"))
            .path_to_test(Workload("higgs").path_to_dataset("higgs_500t_test.csv"))
            .num_features(28)
            .num_blocks(num_blocks)
            .regression()
            .on_error(
                "Make sure that higgs dataset is downloaded and extracted and DATASETSROOT variable is set correctly.")
            .load(numeric_table_type);
    }
};

} // namespace datasets
} // namespace dalbench
