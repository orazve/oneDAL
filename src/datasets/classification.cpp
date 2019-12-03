/** file classification.cpp */
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

class DATASET(SUSY) {
  Dataset load(NumericTableType numeric_table_type, size_t num_blocks) override {
    return DatasetFromCsv()
      .path_to_train(Workload("susy").path_to_dataset("susy_train.csv"))
      .num_features(18)
      .num_responses(2)
      .num_blocks(num_blocks)
      .on_error("Make sure that susy dataset is downloaded and extracted.")
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
      .on_error("Make sure that mnist dataset is downloaded and extracted.")
      .load(numeric_table_type);
  }
};

} // namespace datasets
} // namespace dalbench
