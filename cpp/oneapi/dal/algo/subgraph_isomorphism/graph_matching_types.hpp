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
template <typename Task>
class graph_matching_input_impl;

template <typename Task>
class graph_matching_result_impl;
} // namespace v1

using v1::graph_matching_input_impl;
using v1::graph_matching_result_impl;

} // namespace detail

namespace v1 {

template <typename Task = task::by_default>
class graph_matching_input : public base {
    static_assert(detail::is_valid_task_v<Task>);

public:
    using task_t = Task;

    graph_matching_input(const table& data);
    graph_matching_input(const table& data, const table& initial_centroids);

    const table& get_data() const;

    auto& set_data(const table& data) {
        set_data_impl(data);
        return *this;
    }

    const table& get_initial_centroids() const;

    auto& set_initial_centroids(const table& data) {
        set_initial_centroids_impl(data);
        return *this;
    }

protected:
    void set_data_impl(const table& data);
    void set_initial_centroids_impl(const table& data);

private:
    dal::detail::pimpl<detail::graph_matching_input_impl<Task>> impl_;
};

template <typename Task = task::by_default>
class graph_matching_result {
    static_assert(detail::is_valid_task_v<Task>);

public:
    using task_t = Task;

    graph_matching_result();

    const model<Task>& get_model() const;

    auto& set_model(const model<Task>& value) {
        set_model_impl(value);
        return *this;
    }

    const table& get_labels() const;

    auto& set_labels(const table& value) {
        set_labels_impl(value);
        return *this;
    }

    int64_t get_iteration_count() const;

    auto& set_iteration_count(std::int64_t value) {
        set_iteration_count_impl(value);
        return *this;
    }

    double get_objective_function_value() const;

    auto& set_objective_function_value(double value) {
        set_objective_function_value_impl(value);
        return *this;
    }

protected:
    void set_model_impl(const model<Task>&);
    void set_labels_impl(const table&);
    void set_iteration_count_impl(std::int64_t);
    void set_objective_function_value_impl(double);

private:
    dal::detail::pimpl<detail::graph_matching_result_impl<Task>> impl_;
};

} // namespace v1

using v1::graph_matching_input;
using v1::graph_matching_result;

} // namespace oneapi::dal::preview::subgraph_isomorphism
