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

#include <iostream>

#include "example_util/output_helpers_graph.hpp"
#include "example_util/utils.hpp"
#include "oneapi/dal/algo/subgraph_isomorphism.hpp"
#include "oneapi/dal/graph/undirected_adjacency_array_graph.hpp"
#include "oneapi/dal/io/graph_csv_data_source.hpp"
#include "oneapi/dal/io/load_graph.hpp"
#include "oneapi/dal/table/common.hpp"

namespace dal = oneapi::dal;

int main(int argc, char** argv) {
    const auto filename = get_data_path("graph.csv");

    // read the graph
    const dal::preview::graph_csv_data_source ds_target(filename_target); // n vertices
    const dal::preview::load_graph::descriptor<> d_target;
    const auto target_graph = dal::preview::load_graph::load(d_target, ds_target);

    const dal::preview::graph_csv_data_source ds_pattern(filename_pattern); // m vertices
    const dal::preview::load_graph::descriptor<> d_pattern;
    const auto pattern_graph = dal::preview::load_graph::load(d_pattern, ds_pattern);

    // set algorithm parameters
    const auto subgraph_isomorphism_desc = dal::preview::subgraph_isomorphism::descriptor<>()
                                               .set_induced_mode(true)
                                               .set_semantical_mode(true)
                                               .set_max_match_count(100);

    // compute subgraph_isomorphism similarity coefficients
    const auto result =
        dal::preview::graph_matching(subgraph_isomorphism_desc, target_graph, pattern_graph);

    // extract the result
    const auto match_count = result.get_match_count(); // k

    // table [(match_count + 1) x m]
    const auto vertex_match_table = result.get_vertex_match();

    dal::row_accessor<const std::int32_t> v_acc{ vertex_match };

    for (std::int64_t match_idx = 0; match_idx <= match_count; match_idx++) {
        if (match_idx == 0)
            std::cout << "pattern vertices: ";
        else
            std::cout << "match #" << match_idx << ": ";

        const auto match_values = v_acc.pull(match_idx);
        for (std::int64_t i = 0; i < match_values.get_count(); i++) {
            std::cout << match_values[i] << ", ";
        }
        std::cout << std::endl;
    }
}
