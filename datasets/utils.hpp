#ifndef DAALBENCH_DATASETS_UTILS_HPP_
#define DAALBENCH_DATASETS_UTILS_HPP_

#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#include "daalbench/config.hpp"
#include "daalbench/dataset.hpp"

namespace daalbench {
namespace datasets {

template<typename Delimeter>
inline std::string Join(const std::vector<std::string> &chunks,
                        const Delimeter &delimeter) {
  if (chunks.empty()) {
    return std::string();
  }

  std::stringstream stream;

  stream << chunks.front();
  for (size_t i = 1; i < chunks.size(); i++) {
    if (!chunks[i].empty()) {
      stream << delimeter << chunks[i];
    }
  }
  return stream.str();
}

inline std::string JoinPath(const std::vector<std::string> &chunks) {
  return Join(chunks, '/');
}

inline std::string JoinSentences(const std::vector<std::string> &chunks) {
  return Join(chunks, ". ");
}

inline bool CanOpenFile(const std::string &path) {
  return std::ifstream(path, std::fstream::in).is_open();
}

class Workload {
public:
  explicit Workload(const std::string &name)
    : name_(name) { }

  std::string Path() const {
    const auto &root_path = GlobalConfig::Instance().RootPath();
    return JoinPath({ root_path, "workloads", name_ });
  }

  std::string PathToDataset(const std::string &file_name) const {
    const auto &root_path = GlobalConfig::Instance().RootPath();
    return JoinPath({ root_path, "workloads", name_, "dataset", file_name });
  }

private:
  std::string name_;
};

class DatasetFromCSV {
public:
  DatasetFromCSV &PathToFull(const std::string &value) {
    path_to_full_ = value; return *this;
  }

  DatasetFromCSV &PathToTrain(const std::string &value) {
    path_to_train_ = value; return *this;
  }

  DatasetFromCSV &PathToTest(const std::string &value) {
    path_to_test_ = value; return *this;
  }

  DatasetFromCSV &PathToIndex(const std::string &value) {
    path_to_index_ = value; return *this;
  }

  DatasetFromCSV &FeaturesNumber(size_t features_num) {
    features_num_ = features_num; return *this;
  }

  DatasetFromCSV &ResponsesNumber(size_t responses_num) {
    responses_num_ = responses_num; return *this;
  }

  DatasetFromCSV &TriesNumber(size_t tries_num) {
    tries_num_ = tries_num; return *this;
  }

  DatasetFromCSV &Regression() {
    responses_num_ = 1; return *this;
  }

  DatasetFromCSV &Unlabeled() {
    labeled_ = false; return *this;
  }

  DatasetFromCSV &OnError(const std::string &message) {
    on_error_message_ = message; return *this;
  }

  Dataset Load() {
    const auto train_slice = LoadSlice(path_to_train_);
    const auto test_slice  = LoadSlice(path_to_test_);
    const auto full_slice  = LoadSlice(path_to_full_);
    const auto index_slice  = LoadSlice(path_to_index_);
    return Dataset(train_slice, test_slice, full_slice, index_slice)
      .ResponsesNumber(responses_num_)
      .TriesNumber(tries_num_);
  }

private:
  DataSlice LoadSlice(const std::string &path) {
    if (path.empty()) {
      return DataSlice::MakeEmpty();
    }

    if (!CanOpenFile(path)) {
      throw CannotLoadDataset(JoinSentences(
        { "Cannot open dataset file '" + path + "'", on_error_message_ }
      ));
    }

    return (labeled_ && responses_num_ > 0)
      ? LoadWithResponseVariable(path)
      : LoadNoResponseVariable(path);
  }

  DataSlice LoadNoResponseVariable(const std::string &path) {
    using namespace daal::data_management;
    const CsvDataSourceOptions csv_options = CsvDataSourceOptions::allocateNumericTable |
                                             CsvDataSourceOptions::createDictionaryFromContext;
    FileDataSource<CSVFeatureManager> ds(path, csv_options); ds.loadDataBlock();
    CheckDataSourceStatus(path, ds.status());

    return DataSlice(ds.getNumericTable());
  }

  DataSlice LoadWithResponseVariable(const std::string &path) {
    using namespace daal::data_management;
    if (features_num_ == 0) {
      throw CannotLoadDataset("Cannot load dataset '" + path + "' with responses. "
                              "Number of features undefined. To load CSV dataset with "
                              "responses FeaturesNumber must be specified.");
    }

    auto x = HomogenNumericTable<fptype>::create(features_num_, 0, NumericTable::notAllocate);
    auto y = HomogenNumericTable<fptype>::create(1, 0, NumericTable::notAllocate);
    auto xy = MergedNumericTable::create(x, y);

    const CsvDataSourceOptions csv_options = CsvDataSourceOptions::createDictionaryFromContext;
    FileDataSource<CSVFeatureManager> ds(path, csv_options); ds.loadDataBlock(xy.get());
    CheckDataSourceStatus(path, ds.status());

    return DataSlice(x, y);
  }

  void CheckDataSourceStatus(const std::string &path,
                             const daal::services::Status &status) const {
    if (!status) {
      throw CannotLoadDataset("Cannot read CSV file '" + path + "'");
    }
  }

  size_t features_num_ = 0;
  size_t responses_num_ = 0;
  size_t tries_num_ = 1;
  bool labeled_ = true;
  std::string path_to_train_;
  std::string path_to_test_;
  std::string path_to_full_;
  std::string path_to_index_;
  std::string on_error_message_;
};

} // namespace datasets
} // namespace daalbench

#endif // DAALBENCH_DATASETS_UTILS_HPP_
