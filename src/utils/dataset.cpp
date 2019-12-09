/** file dataset.cpp */
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

NumericTablePtr NumericTableFactory::create_numeric_table(
  const NumericTableType numeric_table_type,
  const size_t num_features,
  const size_t num_observations,
  const NumericTable::AllocationFlag memory_allocation_flag) {
  switch (numeric_table_type) {
    case NumericTableType::SyclHomogenNumericTableFloat:
      return HomogenNumericTable<float>::create(num_features,
                                                num_observations,
                                                memory_allocation_flag);
    case NumericTableType::SyclHomogenNumericTableDouble:
      return HomogenNumericTable<double>::create(num_features,
                                                 num_observations,
                                                 memory_allocation_flag);
    default:
      throw NotAvailableNumericTable("The given numeric table type is not implemented");
      break;
  }
}

DataSlice::DataSlice(const NumericTablePtr& x,
                     const size_t num_blocks,
                     NumericTableType numeric_table_type)
    : x_blocks_(num_blocks),
      labeled_(false) {
  initialize(numeric_table_type, num_blocks, x);
}

DataSlice::DataSlice(const NumericTablePtr& x,
                     const NumericTablePtr& y,
                     const size_t num_blocks,
                     NumericTableType numeric_table_type)
    : x_blocks_(num_blocks),
      y_blocks_(num_blocks),
      labeled_(true) {
  initialize(numeric_table_type, num_blocks, x, y);
}

DataSlice DataSlice::make_empty() {
  return DataSlice();
}

NumericTablePtr DataSlice::x() const {
  if (x_blocks_.empty()) {
    throw EmptyNumericTable("Dataset does not contain X slice");
  }
  return x_blocks_.back();
}

NumericTablePtr DataSlice::y() const {
  if (y_blocks_.empty()) {
    throw EmptyNumericTable("Dataset does not contain Y slice");
  }
  return y_blocks_.back();
}

NumericTablePtr DataSlice::x_block(const size_t block_index) const {
  if ((x_blocks_.empty()) || (!x_blocks_[block_index].get())) {
    throw EmptyNumericTable("Dataset does not contain X slice");
  }
  return x_blocks_[block_index];
}

NumericTablePtr DataSlice::y_block(const size_t block_index) const {
  if ((y_blocks_.empty()) || (!y_blocks_[block_index].get())) {
    throw EmptyNumericTable("Dataset does not contain Y slice");
  }
  return y_blocks_[block_index];
}

NumericTablePtr DataSlice::xy() const {
  using namespace daal::data_management;

  if ((x_blocks_.empty()) && (y_blocks_.empty())) {
    throw EmptyNumericTable("Dataset does not contain neither X nor Y slices");
  }
  else {
    return MergedNumericTable::create(x_blocks_.back(), y_blocks_.back());
  }

  if ((x_blocks_.empty()) && (!y_blocks_.empty())) {
    throw EmptyNumericTable("Dataset does not contain X slice");
  }

  if ((!x_blocks_.empty()) && (y_blocks_.empty())) {
    throw EmptyNumericTable("Dataset does not contain Y slice");
  }
}

NumericTablePtr DataSlice::xy_blocks(const size_t block_index) const {
  using namespace daal::data_management;

  if (((x_blocks_.empty()) && (y_blocks_.empty())) ||
      ((!x_blocks_[block_index].get()) && ((!y_blocks_[block_index].get())))) {
    throw EmptyNumericTable("Dataset does not contain neither X nor Y slices");
  }
  else {
    return MergedNumericTable::create(x_blocks_[block_index], y_blocks_[block_index]);
  }

  if ((x_blocks_.empty()) || (!x_blocks_[block_index].get())) {
    throw EmptyNumericTable("Dataset does not contain X slice");
  }

  if ((y_blocks_.empty()) || (!y_blocks_[block_index].get())) {
    throw EmptyNumericTable("Dataset does not contain Y slice");
  }
}

bool DataSlice::empty() const {
  if (labeled_) {
    return (x_blocks_.empty()) || (y_blocks_.empty());
  }
  else {
    return x_blocks_.empty();
  }
}

void DataSlice::initialize(NumericTableType numeric_table_type,
                           const size_t num_blocks,
                           const NumericTablePtr& x,
                           const NumericTablePtr& y) {
  if (1 == num_blocks) {
    x_blocks_[0] = x;
    if (y.get()) {
      y_blocks_[0] = y;
    }
  }
  else {
    const size_t num_rows   = x->getNumberOfRows();
    const size_t block_size = std::ceil(float(num_rows) / num_blocks);

    for (size_t block_index = 0; block_index < num_blocks; ++block_index) {
      set_block(block_index, block_size, x, y, numeric_table_type);
    }
  }
}

void DataSlice::set_block(const size_t block_index,
                          const size_t block_size,
                          const NumericTablePtr& x,
                          const NumericTablePtr& y,
                          NumericTableType numeric_table_type) {
  x_blocks_[block_index] = copy_block(x, numeric_table_type, block_index, block_size);
  if (y.get()) {
    y_blocks_[block_index] = copy_block(y, numeric_table_type, block_index, block_size);
  }
}

NumericTablePtr DataSlice::copy_block(const NumericTablePtr& numeric_table,
                                      const NumericTableType numeric_table_type,
                                      const size_t block_index,
                                      const size_t block_size) {
  using namespace daal::data_management;
  using FPType = double;

  const size_t num_cols  = numeric_table->getNumberOfColumns();
  const size_t num_rows  = numeric_table->getNumberOfRows();
  const size_t start_row = block_index * block_size;
  size_t end_row         = start_row + block_size;
  if (end_row > num_rows) {
    end_row = num_rows;
  }

  BlockDescriptor<FPType> bd_r;
  numeric_table->getBlockOfRows(start_row, end_row - start_row, ReadWriteMode::readOnly, bd_r);
  FPType* ptr_r = bd_r.getBlockPtr();

  NumericTablePtr numeric_table_tmp =
    NumericTableFactory().create_numeric_table(numeric_table_type,
                                               num_cols,
                                               end_row - start_row,
                                               NumericTable::doAllocate);

  BlockDescriptor<FPType> bd_w;
  numeric_table_tmp->getBlockOfRows(0, end_row - start_row, ReadWriteMode::writeOnly, bd_w);
  FPType* ptr_w = bd_w.getBlockPtr();

  const size_t num_elems = num_cols * (end_row - start_row);
  std::copy(ptr_r, ptr_r + num_elems, ptr_w);

  numeric_table->releaseBlockOfRows(bd_r);
  numeric_table_tmp->releaseBlockOfRows(bd_w);

  return numeric_table_tmp;
}

Dataset::Dataset(const DataSlice& train_slice,
                 const DataSlice& test_slice,
                 const DataSlice& full_slice,
                 const DataSlice& index_slice)
    : train_slice_(train_slice),
      test_slice_(test_slice),
      full_slice_(full_slice),
      index_slice_(index_slice) {
}

Dataset::Dataset(const DataSlice& train_slice,
                 const DataSlice& test_slice,
                 const DataSlice& full_slice)
    : train_slice_(train_slice),
      test_slice_(test_slice),
      full_slice_(full_slice) {
}

Dataset::Dataset(const DataSlice& train_slice, const DataSlice& test_slice)
    : train_slice_(train_slice),
      test_slice_(test_slice) {
}

Dataset::Dataset(const DataSlice& full_slice) : full_slice_(full_slice) {
}

DataSlice Dataset::full() const {
  if (full_slice_.empty()) {
    throw EmptyNumericTable("Full slice of the dataset is empty");
  }
  else {
    return full_slice_;
  }
}

DataSlice Dataset::train() const {
  if (train_slice_.empty()) {
    throw EmptyNumericTable("Train slice of the dataset is empty");
  }
  return train_slice_;
}

DataSlice Dataset::test() const {
  if (test_slice_.empty()) {
    throw EmptyNumericTable("Test slice of the dataset is empty");
  }
  return test_slice_;
}

DataSlice Dataset::index() const {
  if (index_slice_.empty()) {
    throw EmptyNumericTable("Index slice of the dataset is empty");
  }
  return index_slice_;
}

DataSlice Dataset::full_or_train() const {
  if (has_full()) {
    return full();
  }

  if (has_train()) {
    return train();
  }

  throw EmptyNumericTable("Full and Train slices of the dataset are empty");
}

DataSlice Dataset::full_or_test() const {
  if (has_full()) {
    return full();
  }

  if (has_test()) {
    return test();
  }

  throw EmptyNumericTable("Full and Test slices of the dataset are empty");
}

Dataset& Dataset::num_responses(size_t num_responses) {
  num_responses_ = num_responses;
  return *this;
}

Dataset& Dataset::num_tries(size_t num_tries) {
  num_tries_ = num_tries;
  return *this;
}

size_t Dataset::num_responses() const {
  return num_responses_;
}

size_t Dataset::num_tries() const {
  return num_tries_;
}

bool Dataset::has_full() const {
  return !full_slice_.empty();
}

bool Dataset::has_train() const {
  return !train_slice_.empty();
}

bool Dataset::has_test() const {
  return !test_slice_.empty();
}

Workload::Workload(const std::string& name) : name_(name) {
}

std::string Workload::path() const {
  const auto& root_path = GlobalConfig::instance().root_path();
  return join_path({ root_path, "workloads", name_ });
}

std::string Workload::path_to_dataset(const std::string& file_name) const {
  const auto& root_path = GlobalConfig::instance().root_path();
  return join_path({ root_path, "workloads", name_, "dataset", file_name });
}

DatasetFromCsv& DatasetFromCsv::path_to_full(const std::string& value) {
  path_to_full_ = value;
  return *this;
}

DatasetFromCsv& DatasetFromCsv::path_to_train(const std::string& value) {
  path_to_train_ = value;
  return *this;
}

DatasetFromCsv& DatasetFromCsv::path_to_test(const std::string& value) {
  path_to_test_ = value;
  return *this;
}

DatasetFromCsv& DatasetFromCsv::path_to_index(const std::string& value) {
  path_to_index_ = value;
  return *this;
}

DatasetFromCsv& DatasetFromCsv::num_features(size_t num_features) {
  num_features_ = num_features;
  return *this;
}

DatasetFromCsv& DatasetFromCsv::num_responses(size_t num_responses) {
  num_responses_ = num_responses;
  return *this;
}

DatasetFromCsv& DatasetFromCsv::num_blocks(size_t num_blocks) {
  num_blocks_ = num_blocks;
  return *this;
}

DatasetFromCsv& DatasetFromCsv::num_tries(size_t num_tries) {
  num_tries_ = num_tries;
  return *this;
}

DatasetFromCsv& DatasetFromCsv::regression() {
  num_responses_ = 1;
  return *this;
}

DatasetFromCsv& DatasetFromCsv::unlabeled() {
  labeled_ = false;
  return *this;
}

DatasetFromCsv& DatasetFromCsv::on_error(const std::string& message) {
  on_error_message_ = message;
  return *this;
}

Dataset DatasetFromCsv::load(NumericTableType numeric_table_type) {
  const auto train_slice = load_slice(path_to_train_, numeric_table_type);
  const auto test_slice  = load_slice(path_to_test_, numeric_table_type);
  const auto full_slice  = load_slice(path_to_full_, numeric_table_type);
  const auto index_slice = load_slice(path_to_index_, numeric_table_type);
  return Dataset(train_slice, test_slice, full_slice, index_slice)
    .num_responses(num_responses_)
    .num_tries(num_tries_);
}

DataSlice DatasetFromCsv::load_slice(const std::string& path, NumericTableType numeric_table_type) {
  if (path.empty()) {
    return DataSlice::make_empty();
  }

  if (!can_open_file(path)) {
    throw CannotOpenFile(
      join_sentences({ "Cannot open dataset file '" + path + "'", on_error_message_ }));
  }

  return (labeled_ && num_responses_ > 0) ? load_with_response_variable(path, numeric_table_type) :
                                            load_no_response_variable(path, numeric_table_type);
}

DataSlice DatasetFromCsv::load_no_response_variable(const std::string& path,
                                                    NumericTableType numeric_table_type) {
  using namespace daal::data_management;

  const CsvDataSourceOptions csv_options = CsvDataSourceOptions::createDictionaryFromContext;

  auto x = NumericTableFactory().create_numeric_table(numeric_table_type, num_features_);

  FileDataSource<CSVFeatureManager> ds(path, csv_options);
  ds.loadDataBlock(x.get());
  check_data_source_status(path, ds.status());

  return DataSlice(x, num_blocks_, numeric_table_type);
}

DataSlice DatasetFromCsv::load_with_response_variable(const std::string& path,
                                                      NumericTableType numeric_table_type) {
  using namespace daal::data_management;

  if (num_features_ == 0) {
    throw CannotLoadDataset("Cannot load dataset '" + path +
                            "' with responses. "
                            "Number of features undefined. To load CSV dataset with "
                            "responses FeaturesNumber must be specified.");
  }

  auto x  = NumericTableFactory().create_numeric_table(numeric_table_type, num_features_);
  auto y  = NumericTableFactory().create_numeric_table(numeric_table_type, 1);
  auto xy = MergedNumericTable::create(x, y);

  const CsvDataSourceOptions csv_options = CsvDataSourceOptions::createDictionaryFromContext;
  FileDataSource<CSVFeatureManager> ds(path, csv_options);
  ds.loadDataBlock(xy.get());
  check_data_source_status(path, ds.status());

  return DataSlice(x, y, num_blocks_, numeric_table_type);
}

void DatasetFromCsv::check_data_source_status(const std::string& path,
                                              const daal::services::Status& status) const {
  if (!status) {
    throw CannotReadCsv("Cannot read CSV file '" + path + "'");
  }
}

} // namespace dalbench
