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

#pragma once

#include "oneapi/dal/algo/subgraph_isomorphism/common.hpp"

namespace oneapi::dal::preview::subgraph_isomorphism {

namespace detail {
namespace v1 {
template <typename Graph, typename Task = task::by_default>
class graph_matching_input_impl;

template <typename Graph, typename Task = task::by_default>
class graph_matching_result_impl;
} // namespace v1

using v1::graph_matching_input_impl;
using v1::graph_matching_result_impl;

} // namespace detail

namespace v1 {

template <typename Graph, typename Task = task::by_default>
class graph_matching_input : public base {
    static_assert(detail::is_valid_task_v<Task>);

public:
    using task_t = Task;

    graph_matching_input(const Graph& target, const Graph& pattern);

    const Graph& get_target() const;
    const Graph& get_pattern() const;

    auto& set_target(const Graph& target) {
        set_target_impl(data);
        return *this;
    }

    auto& set_pattern(const Graph& pattern) {
        set_pattern_impl(data);
        return *this;
    }

protected:
    void set_target_impl(const Graph& data);
    void set_pattern_impl(const Graph& data);

private:
    dal::detail::pimpl<detail::graph_matching_input_impl<Graph, Task>> impl_;
};

template <typename Task = task::by_default>
class graph_matching_result {
    static_assert(detail::is_valid_task_v<Task>);

public:
    using task_t = Task;

    graph_matching_result();
    graph_matching_result(const table& vertex_mapping, std::int64_t match_count);

    table get_vertex_mapping() const;
    std::int64_t get_match_count() const;

    auto& set_vertex_mapping(const table& value);
    auto& set_match_count(std::int64_t value);

private:
    dal::detail::pimpl<detail::graph_matching_result_impl<Task>> impl_;
};

} // namespace v1

using v1::graph_matching_input;
using v1::graph_matching_result;

} // namespace oneapi::dal::preview::subgraph_isomorphism
