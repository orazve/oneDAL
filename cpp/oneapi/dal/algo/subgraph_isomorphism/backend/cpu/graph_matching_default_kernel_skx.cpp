/*******************************************************************************
* Copyright 2020-2021 Intel Corporation
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

#include <immintrin.h>

#include "oneapi/dal/algo/subgraph_isomorphism/backend/cpu/graph_matching_default_kernel.hpp"
#include "oneapi/dal/algo/subgraph_isomorphism/backend/cpu/graph_matching_default_kernel_avx512.hpp"
#include "oneapi/dal/algo/subgraph_isomorphism/common.hpp"
#include "oneapi/dal/backend/dispatcher.hpp"
#include "oneapi/dal/detail/policy.hpp"
#include "oneapi/dal/graph/detail/service_functions_impl.hpp"
#include "oneapi/dal/table/detail/table_builder.hpp"
#include "oneapi/dal/graph/detail/undirected_adjacency_vector_graph_impl.hpp"

namespace oneapi::dal::preview {
namespace subgraph_isomorphism {
namespace detail {

template graph_matching_result call_subgraph_isomorphism_default_kernel_avx512<
    dal::backend::cpu_dispatch_avx512>(const descriptor_base &desc,
                                       const dal::preview::detail::topology<std::int32_t> &data,
                                       void *result_ptr);

template <>
graph_matching_result
call_subgraph_isomorphism_default_kernel_int32<dal::backend::cpu_dispatch_avx512>(
    const descriptor_base &desc,
    const dal::preview::detail::topology<std::int32_t> &data,
    void *result_ptr) {
    return call_subgraph_isomorphism_default_kernel_avx512<dal::backend::cpu_dispatch_avx512>(
        desc,
        data,
        result_ptr);
}
} // namespace detail
} // namespace subgraph_isomorphism
} // namespace oneapi::dal::preview