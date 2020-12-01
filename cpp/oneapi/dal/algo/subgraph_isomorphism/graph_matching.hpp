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

#include "oneapi/dal/algo/subgraph_isomorphism/detail/graph_matching_ops.hpp"
#include "oneapi/dal/algo/subgraph_isomorphism/graph_matching_types.hpp"
#include "oneapi/dal/graph_matching.hpp"

namespace oneapi::dal::preview::detail {
namespace v1 {

template <typename Descriptor>
struct graph_matching_ops<Descriptor, dal::preview::subgraph_isomorphism::detail::descriptor_tag>
        : dal::preview::subgraph_isomorphism::detail::graph_matching_ops<Descriptor> {};

} // namespace v1
} // namespace oneapi::dal::preview::detail
