/** file dataset.hpp */
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

#ifndef DALBENCH_INCLUDE_DATASET_HPP_
#define DALBENCH_INCLUDE_DATASET_HPP_

#include <cmath>

#ifdef DPCPP_INTERFACES
  #include "daal_sycl.h"
#else
  #include "daal.h"
#endif

#include "config.hpp"
#include "registry.hpp"
#include "service.hpp"

namespace dalbench {

using daal::data_management::NumericTable;
using daal::data_management::NumericTablePtr;

enum NumericTableType { SyclHomogenNumericTableFloat, SyclHomogenNumericTableDouble };

template <typename FPType>
NumericTableType getSyclHomogenNumericTableType() {
  NumericTableType numeric_table_type;

  if (std::is_same<FPType, float>::value) {
    numeric_table_type = NumericTableType::SyclHomogenNumericTableFloat;
  }
  else if (std::is_same<FPType, double>::value) {
    numeric_table_type = NumericTableType::SyclHomogenNumericTableDouble;
  }

  return numeric_table_type;
}

#define TableType(table_type, fp_type) get##table_type##NumericTableType<fp_type>()

class NumericTableFactory {
public:
  NumericTableFactory() = default;

  template <typename FPType>
#ifdef DPCPP_INTERFACES
  using HomogenNumericTable = typename daal::data_management::SyclHomogenNumericTable<FPType>;
#else
  using HomogenNumericTable = typename daal::data_management::HomogenNumericTable<FPType>;
#endif

  NumericTablePtr create_numeric_table(
    const NumericTableType numeric_table_type,
    const size_t num_features,
    const size_t num_observations                             = 0,
    const NumericTable::AllocationFlag memory_allocation_flag = NumericTable::notAllocate);
};

class DataSlice {
public:
  DataSlice() = default;

  explicit DataSlice(const NumericTablePtr& x,
                     const size_t num_blocks,
                     NumericTableType numeric_table_type);

  explicit DataSlice(const NumericTablePtr& x,
                     const NumericTablePtr& y,
                     const size_t num_blocks,
                     NumericTableType numeric_table_type);

  static DataSlice make_empty();

  NumericTablePtr x() const;

  NumericTablePtr y() const;

  NumericTablePtr x_block(const size_t block_index) const;

  NumericTablePtr y_block(const size_t block_index) const;

  NumericTablePtr xy() const;

  NumericTablePtr xy_blocks(const size_t block_index) const;

  bool empty() const;

private:
  void initialize(NumericTableType numeric_table_type,
                  const size_t num_blocks,
                  const NumericTablePtr& x,
                  const NumericTablePtr& y = NumericTablePtr());

  void set_block(const size_t block_index,
                 const size_t block_size,
                 const NumericTablePtr& x,
                 const NumericTablePtr& y,
                 NumericTableType numeric_table_type);

  NumericTablePtr copy_block(const NumericTablePtr& numeric_table,
                             const NumericTableType numeric_table_type,
                             const size_t block_index,
                             const size_t block_size);

private:
  std::vector<NumericTablePtr> x_blocks_;
  std::vector<NumericTablePtr> y_blocks_;
  bool labeled_;
};

class Dataset {
public:
  Dataset()  = default;
  ~Dataset() = default;

  explicit Dataset(const DataSlice& train_slice,
                   const DataSlice& test_slice,
                   const DataSlice& full_slice,
                   const DataSlice& index_slice);

  explicit Dataset(const DataSlice& train_slice,
                   const DataSlice& test_slice,
                   const DataSlice& full_slice);

  explicit Dataset(const DataSlice& train_slice, const DataSlice& test_slice);

  explicit Dataset(const DataSlice& full_slice);

  DataSlice full() const;

  DataSlice train() const;

  DataSlice index() const;

  DataSlice test() const;

  DataSlice full_or_train() const;

  DataSlice full_or_test() const;

  Dataset& num_responses(size_t num_responses);

  Dataset& num_tries(size_t num_tries);

  size_t num_responses() const;

  size_t num_tries() const;

  bool has_full() const;

  bool has_train() const;

  bool has_test() const;

private:
  DataSlice train_slice_;
  DataSlice test_slice_;
  DataSlice full_slice_;
  DataSlice index_slice_;
  size_t num_responses_ = 0;
  size_t num_tries_     = 1;
};

class Workload {
public:
  explicit Workload(const std::string& name);

  std::string path() const;
  std::string path_to_dataset(const std::string& file_name) const;

private:
  std::string name_;
};

class DatasetFromCsv {
public:
  DatasetFromCsv& path_to_full(const std::string& value);

  DatasetFromCsv& path_to_train(const std::string& value);

  DatasetFromCsv& path_to_test(const std::string& value);

  DatasetFromCsv& path_to_index(const std::string& value);

  DatasetFromCsv& num_features(size_t num_features);

  DatasetFromCsv& num_responses(size_t num_responses);

  DatasetFromCsv& num_blocks(size_t num_blocks);

  DatasetFromCsv& num_tries(size_t num_tries);

  DatasetFromCsv& regression();

  DatasetFromCsv& unlabeled();

  DatasetFromCsv& on_error(const std::string& message);

  Dataset load(NumericTableType numeric_table_type);

private:
  DataSlice load_slice(const std::string& path, NumericTableType numeric_table_type);

  DataSlice load_no_response_variable(const std::string& path, NumericTableType numeric_table_type);

  DataSlice load_with_response_variable(const std::string& path,
                                        NumericTableType numeric_table_type);

  void check_data_source_status(const std::string& path,
                                const daal::services::Status& status) const;

private:
  size_t num_features_  = 0;
  size_t num_responses_ = 0;
  size_t num_blocks_    = 0;
  size_t num_tries_     = 1;
  bool labeled_         = true;
  std::string path_to_train_;
  std::string path_to_test_;
  std::string path_to_full_;
  std::string path_to_index_;
  std::string on_error_message_;
};

class DatasetLoader {
public:
  virtual ~DatasetLoader()                                                     = default;
  virtual Dataset load(NumericTableType numeric_table_type, size_t num_blocks) = 0;
};

#define DATASET(name)            \
  name;                          \
  REGISTER_DATASET(#name, name); \
  class name : public DatasetLoader

} // namespace dalbench

#endif // DALBENCH_INCLUDE_DATASET_HPP_
