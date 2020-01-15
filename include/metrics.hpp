#include <cmath>
#include <set>
#include <string>
#include <vector>

#include "daal.h"

#include "error_types.hpp"

namespace dalbench {

enum class MetricType { Mse = 1, Accuracy = 2, DaviesBouldinIndex = 3 };
struct MetricParams {
  std::string name;
  double value;
};

struct ProcessedData {
  ProcessedData(const daal::data_management::NumericTablePtr& aux_table,
                const daal::data_management::NumericTablePtr& y_predict)
      : aux_table_(aux_table),
        y_predict_(y_predict) {
    dim = aux_table_->getNumberOfColumns();

    num_rows = y_predict_->getNumberOfRows();
    num_cols = y_predict_->getNumberOfColumns();

    aux_table_->getBlockOfRows(0,
                               num_rows,
                               daal::data_management::ReadWriteMode::readOnly,
                               bd_aux_table_);
    ptr_aux_table = bd_aux_table_.getBlockPtr();

    y_predict_->getBlockOfRows(0,
                               num_rows,
                               daal::data_management::ReadWriteMode::readOnly,
                               bd_y_predict_);
    ptr_y_predict = bd_y_predict_.getBlockPtr();
  }

  ~ProcessedData() {
    if (y_predict_) {
      y_predict_->releaseBlockOfRows(bd_y_predict_);
    }
    if (aux_table_) {
      aux_table_->releaseBlockOfRows(bd_aux_table_);
    }
  }

public:
  size_t num_rows;
  size_t num_cols;
  size_t dim;
  const double* ptr_aux_table;
  const double* ptr_y_predict;

private:
  daal::data_management::BlockDescriptor<double> bd_aux_table_;
  daal::data_management::BlockDescriptor<double> bd_y_predict_;
  const daal::data_management::NumericTablePtr& aux_table_;
  const daal::data_management::NumericTablePtr& y_predict_;
};

template <MetricType metric>
class Metric {
public:
  static MetricParams compute_metric(const daal::data_management::NumericTablePtr& aux_table,
                                     const daal::data_management::NumericTablePtr& y_predict) {
    throw NotAvailableMetric("The given metric is not supported");
  }
};

template <>
class Metric<MetricType::Mse> {
public:
  static MetricParams compute_metric(const daal::data_management::NumericTablePtr& aux_table,
                                     const daal::data_management::NumericTablePtr& y_predict);
};

template <>
class Metric<MetricType::Accuracy> {
public:
  static MetricParams compute_metric(const daal::data_management::NumericTablePtr& aux_table,
                                     const daal::data_management::NumericTablePtr& y_predict);
};

template <>
class Metric<MetricType::DaviesBouldinIndex> {
public:
  static MetricParams compute_metric(const daal::data_management::NumericTablePtr& aux_table,
                                     const daal::data_management::NumericTablePtr& y_predict);
};

} // namespace dalbench
