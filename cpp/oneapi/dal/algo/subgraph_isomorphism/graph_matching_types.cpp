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

#include "oneapi/dal/algo/subgraph_isomorphism/graph_matching_types.hpp"
#include "oneapi/dal/detail/common.hpp"
#include "oneapi/dal/exceptions.hpp"

namespace oneapi::dal::preview::subgraph_isomorphism {

template <typename Graph, typename Task>
class detail::v1::graph_matching_input_impl : public base {
public:
    graph_matching_input_impl(const Graph& target, const Graph& pattern)
            : target(target),
              pattern(pattern) {}

    Graph target;
    Graph pattern;
};

template <typename Task>
class detail::v1::graph_matching_result_impl : public base {
public:
    std::int64_t match_count = 0;
    table vertex_mapping;
    table edge_mapping;
};

using detail::v1::graph_matching_input_impl;
using detail::v1::graph_matching_result_impl;

namespace v1 {

template <typename Graph, typename Task>
const Graph& graph_matching_input<Graph, Task>::get_target() const {
    return /*...*/;
}

template <typename Graph, typename Task>
const Graph& graph_matching_input<Graph, Task>::get_pattern() const {
    return /*...*/;
}

template <typename Graph, typename Task>
const void graph_matching_input<Graph, Task>::set_target_impl() const {
    /*...*/;
}

template <typename Graph, typename Task>
const void graph_matching_input<Graph, Task>::set_pattern_impl() const {
    /*...*/;
}

typedef typename undirected_adjacency_array_graph<empty_value,
                                                  empty_value,
                                                  empty_value,
                                                  std::int32_t,
                                                  std::allocator<char>>
    graph_t;

template class ONEDAL_EXPORT graph_matching_input<graph_t, task::compute>;
template class ONEDAL_EXPORT graph_matching_result<graph_t, task::compute>;

} // namespace v1
} // namespace oneapi::dal::preview::subgraph_isomorphism
