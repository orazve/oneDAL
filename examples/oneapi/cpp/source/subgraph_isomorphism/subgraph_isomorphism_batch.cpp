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

namespace subgraph_isomorphism {
namespace method {
struct vf3lp {};
/* struct ri {}; */
/* struct glasgow {}; */
using by_default = vf3lp;
} // namespace method

namespace task {
struct induced {};
/* struct non_induced {}; */
using by_default = induced;
} // namespace task

template <typename Float = float typename Method = method::by_default,
          typename Task = task::by_default>
class descriptor {
public:
    descriptor(); //

    auto get_max_match_count() const -> std::int64_t;
    auto& set_max_match_count(std::int64_t value);
};
} // namespace subgraph_isomorphism

int main(int argc, char** argv) {
    const auto filename = get_data_path("graph.csv");

    // read the graph
    const dal::preview::graph_csv_data_source ds_target(filename_target);
    const dal::preview::load_graph::descriptor<> d_target;
    const auto target_graph = dal::preview::load_graph::load(d_target, ds_target);

    const dal::preview::graph_csv_data_source ds_pattern(filename_pattern);
    const dal::preview::load_graph::descriptor<> d_pattern;
    const auto pattern_graph = dal::preview::load_graph::load(d_pattern, ds_pattern);

    // set algorithm parameters
    const auto subgraph_isomorphism_desc =
        dal::preview::subgraph_isomorphism::descriptor<>().set_block(
            { row_range_begin, row_range_end },
            { column_range_begin, column_range_end });

    // compute subgraph_isomorphism similarity coefficients
    const auto result_subgraph_isomorphism =
        dal::preview::graph_matching(subgraph_isomorphism_desc, target_graph, pattern_graph);

    // extract the result
    const auto subgraph_isomorphism_coeffs = result_subgraph_isomorphism.get_coeffs();
    const auto vertex_pairs = result_subgraph_isomorphism.get_vertex_pairs();
    const std::int64_t nonzero_coeff_count = result_subgraph_isomorphism.get_nonzero_coeff_count();

    std::cout << "The number of nonzero subgraph_isomorphism coeffs in the block: "
              << nonzero_coeff_count << std::endl;

    print_subgraph_isomorphism_result(result_subgraph_isomorphism);
}
