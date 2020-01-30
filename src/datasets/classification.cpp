/** file classification.cpp */
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

#include "dataset.hpp"

namespace dalbench {
namespace datasets {

class DATASET(SUSY) {
  Dataset load(NumericTableType numeric_table_type, size_t num_blocks) override {
    return DatasetFromCsv()
      .path_to_train(Workload("susy").path_to_dataset("susy_train.csv"))
      .path_to_test(Workload("susy").path_to_dataset("susy_test.csv"))
      .num_features(18)
      .num_responses(2)
      .num_blocks(num_blocks)
      .on_error(
        "Make sure that susy dataset is downloaded and extracted and DATASETSROOT variable is set correctly.")
      .load(numeric_table_type);
  }
};

class DATASET(mnist) {
  Dataset load(NumericTableType numeric_table_type, size_t num_blocks) override {
    return DatasetFromCsv()
      .path_to_train(Workload("mnist").path_to_dataset("mnist_train.csv"))
      .path_to_test(Workload("mnist").path_to_dataset("mnist_test.csv"))
      .num_features(784)
      .num_responses(10)
      .num_blocks(num_blocks)
      .on_error(
        "Make sure that mnist dataset is downloaded and extracted and DATASETSROOT variable is set correctly.")
      .load(numeric_table_type);
  }
};

class DATASET(higgs_small_classifier) {
  Dataset load(NumericTableType numeric_table_type, size_t num_blocks) override {
    return DatasetFromCsv()
      .path_to_train(Workload("higgs").path_to_dataset("higgs_100t_train.csv"))
      .path_to_test(Workload("higgs").path_to_dataset("higgs_50t_test.csv"))
      .num_features(28)
      .num_responses(2)
      .num_blocks(num_blocks)
      .on_error("Make sure that HIGGS dataset is downloaded and extracted.")
      .load(numeric_table_type);
  }
};

class DATASET(hepmass_small_classifier) {
  Dataset load(NumericTableType numeric_table_type, size_t num_blocks) override {
    return DatasetFromCsv()
      .path_to_train(Workload("hepmass").path_to_dataset("hepmass_100t_train.csv"))
      .path_to_test(Workload("hepmass").path_to_dataset("hepmass_50t_test.csv"))
      .num_features(28)
      .num_responses(2)
      .num_blocks(num_blocks)
      .on_error("Make sure that HEPMASS dataset is downloaded and extracted. "
                "To download and preproces dataset run hepmass_load.py located in HEPMASS workload "
                "directory: ")
      .load(numeric_table_type);
  }
};

class DATASET(epsilion_50k_classifier) {
  Dataset load(NumericTableType numeric_table_type, size_t num_blocks) override {
    return DatasetFromCsv()
      .path_to_train(Workload("epsilon").path_to_dataset("epsilon_50k_train.csv"))
      .path_to_test(Workload("epsilon").path_to_dataset("epsilon_50k_train.csv"))
      .num_features(2000)
      .num_responses(2)
      .num_blocks(num_blocks)
      .on_error("Make sure that epsilon dataset is downloaded and extracted.")
      .load(numeric_table_type);
  }
};

} // namespace datasets
} // namespace dalbench
