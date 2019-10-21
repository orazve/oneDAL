#ifndef DAALBENCH_DATASET_HPP_
#define DAALBENCH_DATASET_HPP_

#include "daal.h"

namespace daalbench {

using daal::data_management::NumericTable;
using daal::data_management::NumericTablePtr;

class CannotLoadDataset : public std::runtime_error {
public:
  explicit CannotLoadDataset(const std::string &description)
    : std::runtime_error(description) { }
};

class EmptyDataSliceError : public std::runtime_error {
public:
  explicit EmptyDataSliceError(const std::string &slice_name)
    : std::runtime_error(slice_name + " slice of the dataset is empty") { }
};

class DataSlice {
public:
  static DataSlice MakeEmpty() {
    return DataSlice();
  }

  DataSlice() = default;

  explicit DataSlice(const NumericTablePtr &x)
    : x_(x) { }

  explicit DataSlice(const NumericTablePtr &x,
                     const NumericTablePtr &y)
    : x_(x), y_(y) { }

  NumericTablePtr X() const {
    if (!x_) {
      throw EmptyDataSliceError("Dataset does not contain X slice");
    }
    return x_;
  }

  NumericTablePtr Y() const {
    if (!x_) {
      throw EmptyDataSliceError("Dataset does not contain Y slice");
    }
    return y_;
  }

  NumericTablePtr XY() const {
    using namespace daal::data_management;
    if (x_ && y_) {
      return MergedNumericTable::create(x_, y_);
    }
    if (x_ && !y_) {
      return x_;
    }
    if (!x_ && y_) {
      return y_;
    }

    throw EmptyDataSliceError("Dataset does not contain neither X nor Y slices");
  }

  bool Empty() const {
    return !(x_.get() || y_.get());
  }

private:
  NumericTablePtr x_;
  NumericTablePtr y_;
};

class Dataset {
public:
  explicit Dataset(const DataSlice &train_slice,
                   const DataSlice &test_slice,
                   const DataSlice &full_slice)
    : train_slice_(train_slice),
      test_slice_(test_slice),
      full_slice_(full_slice) { }

  explicit Dataset(const DataSlice &train_slice,
                   const DataSlice &test_slice,
                   const DataSlice &full_slice,
                   const DataSlice &index_slice)
    : train_slice_(train_slice),
      test_slice_(test_slice),
      full_slice_(full_slice),
      index_slice_(index_slice) { }

  explicit Dataset(const DataSlice &train_slice,
                   const DataSlice &test_slice)
    : train_slice_(train_slice),
      test_slice_(test_slice) { }

  explicit Dataset(const DataSlice &full_slice)
    : full_slice_(full_slice) { }

  DataSlice Full() const {
    if (!full_slice_.Empty()) {
      return full_slice_;
    }

    const bool has_train = !train_slice_.Empty();
    const bool has_test = !test_slice_.Empty();

    if (has_train && !has_test) {
      return train_slice_;
    }

    if (!has_train && has_test) {
      return test_slice_;
    }

    // TODO: Merge slices
    throw NotImplementedException("Dataset::Full");
  }

  DataSlice Train() const {
    if (train_slice_.Empty()) {
      throw EmptyDataSliceError("Train");
    }
    return train_slice_;
  }

  DataSlice Index() const {
    if (index_slice_.Empty()) {
      throw EmptyDataSliceError("Index");
    }
    return index_slice_;
  }

  DataSlice Test() const {
    if (test_slice_.Empty()) {
      throw EmptyDataSliceError("Test");
    }
    return test_slice_;
  }

  DataSlice FullOrTrain() const {
    if (HasFull()) {
      return Full();
    }

    if (HasTrain()) {
      return Train();
    }

    throw EmptyDataSliceError("Full or Train");
  }

  Dataset &ResponsesNumber(size_t responses_num) {
    responses_num_ = responses_num; return *this;
  }

  Dataset &TriesNumber(size_t tries_num) {
    tries_num_ = tries_num; return *this;
  }

  size_t ResponsesNumber() const {
    return responses_num_;
  }

  size_t TriesNumber() const {
    return tries_num_;
  }

  bool HasFull() const {
    return !full_slice_.Empty();
  }

  bool HasTrain() const {
    return !train_slice_.Empty();
  }

  bool HasTest() const {
    return !test_slice_.Empty();
  }

private:
  DataSlice train_slice_;
  DataSlice test_slice_;
  DataSlice full_slice_;
  DataSlice index_slice_;
  size_t responses_num_ = 0;
  size_t tries_num_ = 1;
};

class DatasetLoader
{
public:
  virtual ~DatasetLoader() = default;
  virtual Dataset Load() = 0;
};

#define DATASET(name) \
  name; REGISTER_DATASET(#name, name); class name : public DatasetLoader

} // namespace daalbench

#endif // DAALBENCH_DATASET_HPP_
