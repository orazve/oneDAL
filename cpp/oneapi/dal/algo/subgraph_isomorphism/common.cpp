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

#include "oneapi/dal/algo/subgraph_isomorphism/common.hpp"
#include "oneapi/dal/exceptions.hpp"

namespace oneapi::dal::preview::subgraph_isomorphism {
namespace detail {
namespace v1 {

template <typename Task>
class descriptor_impl : public base {
public:
    bool induced_mode = true;
    bool semantical_mode = false;
};

template <typename Task>
descriptor_base<Task>::descriptor_base() : impl_(new descriptor_impl<Task>{}) {}

template <typename Task>
std::int64_t descriptor_base<Task>::get_cluster_count() const {
    return impl_->cluster_count;
}

template <typename Task>
std::int64_t descriptor_base<Task>::get_max_iteration_count() const {
    return impl_->max_iteration_count;
}

template <typename Task>
double descriptor_base<Task>::get_accuracy_threshold() const {
    return impl_->accuracy_threshold;
}

template <typename Task>
void descriptor_base<Task>::set_cluster_count_impl(std::int64_t value) {
    if (value <= 0) {
        throw domain_error(dal::detail::error_messages::cluster_count_leq_zero());
    }
    impl_->cluster_count = value;
}

template <typename Task>
void descriptor_base<Task>::set_max_iteration_count_impl(std::int64_t value) {
    if (value < 0) {
        throw domain_error(dal::detail::error_messages::max_iteration_count_lt_zero());
    }
    impl_->max_iteration_count = value;
}

template <typename Task>
void descriptor_base<Task>::set_accuracy_threshold_impl(double value) {
    if (value < 0.0) {
        throw domain_error(dal::detail::error_messages::accuracy_threshold_lt_zero());
    }
    impl_->accuracy_threshold = value;
}

template class ONEDAL_EXPORT descriptor_base<task::compute>;

} // namespace v1
} // namespace detail

} // namespace oneapi::dal::preview::subgraph_isomorphism
