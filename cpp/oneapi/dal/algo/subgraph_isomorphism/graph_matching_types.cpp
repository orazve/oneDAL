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

template <typename Task>
class detail::v1::graph_matching_input_impl : public base {
public:
    graph_matching_input_impl(const table& data) : data(data) {}
    graph_matching_input_impl(const table& data, const table& initial_centroids)
            : data(data),
              initial_centroids(initial_centroids) {}

    table data;
    table initial_centroids;
};

template <typename Task>
class detail::v1::graph_matching_result_impl : public base {
public:
    model<Task> graph_matchinged_model;
    table labels;
    std::int64_t iteration_count = 0;
    double objective_function_value = 0.0;
};

using detail::v1::graph_matching_input_impl;
using detail::v1::graph_matching_result_impl;

namespace v1 {

template <typename Task>
graph_matching_input<Task>::graph_matching_input(const table& data)
        : impl_(new graph_matching_input_impl<Task>{ data }) {}

template <typename Task>
graph_matching_input<Task>::graph_matching_input(const table& data, const table& initial_centroids)
        : impl_(new graph_matching_input_impl<Task>(data, initial_centroids)) {}

template <typename Task>
const table& graph_matching_input<Task>::get_data() const {
    return impl_->data;
}

template <typename Task>
const table& graph_matching_input<Task>::get_initial_centroids() const {
    return impl_->initial_centroids;
}

template <typename Task>
void graph_matching_input<Task>::set_data_impl(const table& value) {
    impl_->data = value;
}

template <typename Task>
void graph_matching_input<Task>::set_initial_centroids_impl(const table& value) {
    impl_->initial_centroids = value;
}

template <typename Task>
graph_matching_result<Task>::graph_matching_result()
        : impl_(new graph_matching_result_impl<Task>{}) {}

template <typename Task>
const model<Task>& graph_matching_result<Task>::get_model() const {
    return impl_->graph_matchinged_model;
}

template <typename Task>
const table& graph_matching_result<Task>::get_labels() const {
    return impl_->labels;
}

template <typename Task>
std::int64_t graph_matching_result<Task>::get_iteration_count() const {
    return impl_->iteration_count;
}

template <typename Task>
double graph_matching_result<Task>::get_objective_function_value() const {
    return impl_->objective_function_value;
}

template <typename Task>
void graph_matching_result<Task>::set_model_impl(const model<Task>& value) {
    impl_->graph_matchinged_model = value;
}

template <typename Task>
void graph_matching_result<Task>::set_labels_impl(const table& value) {
    impl_->labels = value;
}

template <typename Task>
void graph_matching_result<Task>::set_iteration_count_impl(std::int64_t value) {
    if (value < 0) {
        throw domain_error(dal::detail::error_messages::iteration_count_lt_zero());
    }
    impl_->iteration_count = value;
}

template <typename Task>
void graph_matching_result<Task>::set_objective_function_value_impl(double value) {
    if (value < 0.0) {
        throw domain_error(dal::detail::error_messages::objective_function_value_lt_zero());
    }
    impl_->objective_function_value = value;
}

template class ONEDAL_EXPORT graph_matching_input<task::clustering>;
template class ONEDAL_EXPORT graph_matching_result<task::clustering>;

} // namespace v1
} // namespace oneapi::dal::preview::subgraph_isomorphism
