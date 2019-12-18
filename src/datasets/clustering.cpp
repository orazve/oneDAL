/** file clustering.cpp */
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

class DATASET(higgs_1M_cluster) {
    Dataset load(NumericTableType numeric_table_type, size_t num_blocks) override {
        return DatasetFromCsv()
            .path_to_full(Workload("higgs").path_to_dataset("higgs_1m_train.csv"))
            .num_features(28)
            .num_responses(2)
            .num_blocks(num_blocks)
            .on_error(
                "Make sure that higgs dataset is downloaded and extracted and DATASETSROOT variable is set correctly.")
            .load(numeric_table_type);
    }
};

class DATASET(hepmass_1M_cluster) {
    Dataset load(NumericTableType numeric_table_type, size_t num_blocks) override {
        return DatasetFromCsv()
            .path_to_full(Workload("hepmass").path_to_dataset("hepmass_1m_train.csv"))
            .num_features(28)
            .num_responses(2)
            .num_blocks(num_blocks)
            .on_error(
                "Make sure that hepmass dataset is downloaded and extracted and DATASETSROOT variable is set correctly.")
            .load(numeric_table_type);
    }
};

class DATASET(SUSY_cluster) {
    Dataset load(NumericTableType numeric_table_type, size_t num_blocks) override {
        return DatasetFromCsv()
            .path_to_full(Workload("susy").path_to_dataset("susy_train.csv"))
            .num_features(18)
            .num_responses(2)
            .num_blocks(num_blocks)
            .on_error(
                "Make sure that susy dataset is downloaded and extracted and DATASETSROOT variable is set correctly.")
            .load(numeric_table_type);
    }
};

} // namespace datasets
} // namespace dalbench
