/*******************************************************************************
* Copyright 2020 Intel Corporation
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

#include "gtest/gtest.h"
#include "oneapi/dal/algo/subgraph_isomorphism/infer.hpp"
#include "oneapi/dal/algo/subgraph_isomorphism/graph_matching.hpp"
#include "oneapi/dal/table/row_accessor.hpp"

using namespace oneapi::dal;

TEST(subgraph_isomorphism_vf3lp_dense_cpu, graph_matching_results) {
    constexpr std::int64_t row_count = 8;
    constexpr std::int64_t column_count = 2;
    constexpr std::int64_t cluster_count = 2;

    const float data[] = { 1.0,  1.0,  2.0,  2.0,  1.0,  2.0,  2.0,  1.0,
                           -1.0, -1.0, -1.0, -2.0, -2.0, -1.0, -2.0, -2.0 };

    const int labels[] = { 1, 1, 1, 1, 0, 0, 0, 0 };

    const float centroids[] = { -1.5, -1.5, 1.5, 1.5 };

    const auto data_table = homogen_table::wrap(data, row_count, column_count);

    const auto subgraph_isomorphism_desc = subgraph_isomorphism::descriptor<>()
                                               .set_cluster_count(cluster_count)
                                               .set_max_iteration_count(4)
                                               .set_accuracy_threshold(0.001);

    const auto result_graph_matching = graph_matching(subgraph_isomorphism_desc, data_table);

    const auto graph_matching_labels =
        row_accessor<const int>(result_graph_matching.get_labels()).pull().get_data();
    for (std::int64_t i = 0; i < row_count; ++i) {
        ASSERT_EQ(labels[i], graph_matching_labels[i]);
    }

    const auto graph_matching_centroids =
        row_accessor<const float>(result_graph_matching.get_model().get_centroids())
            .pull()
            .get_data();
    for (std::int64_t i = 0; i < cluster_count * column_count; ++i) {
        ASSERT_FLOAT_EQ(centroids[i], graph_matching_centroids[i]);
    }
}

TEST(subgraph_isomorphism_vf3lp_dense_cpu, infer_results) {
    constexpr std::int64_t row_count = 8;
    constexpr std::int64_t column_count = 2;
    constexpr std::int64_t cluster_count = 2;

    const float data[] = { 1.0,  1.0,  2.0,  2.0,  1.0,  2.0,  2.0,  1.0,
                           -1.0, -1.0, -1.0, -2.0, -2.0, -1.0, -2.0, -2.0 };

    const auto data_table = homogen_table::wrap(data, row_count, column_count);

    const auto subgraph_isomorphism_desc = subgraph_isomorphism::descriptor<>()
                                               .set_cluster_count(cluster_count)
                                               .set_max_iteration_count(4)
                                               .set_accuracy_threshold(0.001);

    const auto result_graph_matching = graph_matching(subgraph_isomorphism_desc, data_table);

    constexpr std::int64_t infer_row_count = 9;
    const float data_infer[] = { 1.0, 1.0,  0.0, 1.0,  1.0,  0.0,  2.0, 2.0,  7.0,
                                 0.0, -1.0, 0.0, -5.0, -5.0, -5.0, 0.0, -2.0, 1.0 };
    const auto data_infer_table = homogen_table::wrap(data_infer, infer_row_count, column_count);

    const int infer_labels[] = { 1, 1, 1, 1, 1, 0, 0, 0, 0 };

    const auto result_test =
        infer(subgraph_isomorphism_desc, result_graph_matching.get_model(), data_infer_table);

    const auto test_labels = row_accessor<const int>(result_test.get_labels()).pull().get_data();
    for (std::int64_t i = 0; i < infer_row_count; ++i) {
        ASSERT_EQ(infer_labels[i], test_labels[i]);
    }
}
