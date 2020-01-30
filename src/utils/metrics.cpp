/** file metrics.cpp */
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

#include "metrics.hpp"

namespace dalbench {

MetricParams Metric<MetricType::Mse>::compute_metric(
  const daal::data_management::NumericTablePtr& aux_table,
  const daal::data_management::NumericTablePtr& y_predict) {
  ProcessedData data(aux_table, y_predict);

  const size_t size = data.num_rows * data.num_cols;
  double sum        = 0.0;
  for (size_t elem_idx = 0; elem_idx < size; ++elem_idx) {
    sum += ((data.ptr_y_predict[elem_idx] - data.ptr_aux_table[elem_idx]) *
            (data.ptr_y_predict[elem_idx] - data.ptr_aux_table[elem_idx]));
  }

  const double mse = sum / size;

  return MetricParams{ "MSE", mse };
}

MetricParams Metric<MetricType::Accuracy>::compute_metric(
  const daal::data_management::NumericTablePtr& aux_table,
  const daal::data_management::NumericTablePtr& y_predict) {
  ProcessedData data(aux_table, y_predict);

  const size_t size = data.num_rows * data.num_cols;
  double sum        = 0.0;

  if (1 == data.num_cols) {
    for (size_t elem_idx = 0; elem_idx < size; ++elem_idx) {
      sum += (data.ptr_y_predict[elem_idx] == data.ptr_aux_table[elem_idx]);
    }
  }
  else {
    double max_val = 0.0;
    size_t max_arg = 0;
    for (size_t row_idx = 0; row_idx < data.num_rows; ++row_idx) {
      for (size_t col_idx = 0; col_idx < data.num_cols; ++col_idx) {
        if (max_val < data.ptr_y_predict[row_idx * data.num_cols + col_idx]) {
          max_val = data.ptr_y_predict[row_idx * data.num_cols + col_idx];
          max_arg = col_idx;
        }
      }
      sum += static_cast<double>(static_cast<double>(max_arg) == data.ptr_aux_table[row_idx]);
    }
  }
  const double accuracy = sum / size;

  return MetricParams{ "Accuracy", accuracy };
}

MetricParams Metric<MetricType::DaviesBouldinIndex>::compute_metric(
  const daal::data_management::NumericTablePtr& aux_table,
  const daal::data_management::NumericTablePtr& y_predict) {
  ProcessedData data(aux_table, y_predict);

  size_t size = data.num_rows * data.num_cols;
  std::set<double> cluster_indices(data.ptr_y_predict, data.ptr_y_predict + size);
  size_t num_clusters = cluster_indices.size();
  cluster_indices.clear();

  std::vector<double> dispersions(num_clusters, 0.0);
  std::vector<double> centroids(num_clusters * data.dim, 0.0);

  for (size_t cluster_idx = 0; cluster_idx < num_clusters; ++cluster_idx) {
    std::vector<double> x_k;
    for (size_t assignment_idx = 0; assignment_idx < data.num_rows; ++assignment_idx) {
      if (cluster_idx == data.ptr_y_predict[assignment_idx]) {
        for (size_t col = 0; col < data.dim; ++col) {
          x_k.push_back(data.ptr_aux_table[assignment_idx * data.dim + col]);
        }
      }
    }

    size_t row_count = x_k.size() / data.dim;
    for (size_t col = 0; col < data.dim; ++col) {
      double sum_per_col = 0.0;
      for (size_t row = 0; row < row_count; ++row) {
        sum_per_col += x_k[row * data.dim + col];
      }
      centroids[cluster_idx * data.dim + col] = sum_per_col / row_count;
    }

    double sum = 0.0;
    for (size_t row = 0; row < row_count; ++row) {
      for (size_t col = 0; col < data.dim; ++col) {
        sum += ((x_k[row * data.dim + col] - centroids[col]) *
                (x_k[row * data.dim + col] - centroids[col]));
      }
    }
    dispersions[cluster_idx] = std::sqrt(sum) / row_count;
  }

  double DBI = 0.0;
  for (size_t cluster_i = 0; cluster_i < num_clusters; ++cluster_i) {
    double max_ratio = 0.0;
    for (size_t cluster_j = 0; cluster_j < num_clusters; ++cluster_j) {
      if (cluster_i != cluster_j) {
        double m_ij = 0.0;
        for (size_t col = 0; col < data.dim; ++col) {
          m_ij += ((centroids[cluster_i * data.dim + col] - centroids[cluster_j * data.dim + col]) *
                   (centroids[cluster_i * data.dim + col] - centroids[cluster_j * data.dim + col]));
        }
        const double s_ij  = dispersions[cluster_i] + dispersions[cluster_j];
        const double ratio = s_ij / std::sqrt(m_ij);
        max_ratio          = std::max(max_ratio, ratio);
      }
      DBI += max_ratio;
    }
  }

  DBI /= num_clusters;

  return MetricParams{ "DBI", DBI };
}

template class Metric<MetricType::Mse>;
template class Metric<MetricType::Accuracy>;
template class Metric<MetricType::DaviesBouldinIndex>;

}; // namespace dalbench
