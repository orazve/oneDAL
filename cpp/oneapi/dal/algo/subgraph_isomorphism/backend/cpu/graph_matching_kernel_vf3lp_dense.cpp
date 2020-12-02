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

#include <daal/src/algorithms/subgraph_isomorphism/subgraph_isomorphism_vf3lp_kernel.h>

#include "oneapi/dal/algo/subgraph_isomorphism/backend/cpu/graph_matching_kernel.hpp"
#include "oneapi/dal/backend/interop/common.hpp"
#include "oneapi/dal/backend/interop/error_converter.hpp"
#include "oneapi/dal/backend/interop/table_conversion.hpp"
#include "oneapi/dal/exceptions.hpp"

#include "oneapi/dal/table/row_accessor.hpp"

namespace oneapi::dal::preview::subgraph_isomorphism::backend {

using std::int64_t;
using dal::backend::context_cpu;
using descriptor_t = detail::descriptor_base<task::compute>;

namespace daal_subgraph_isomorphism = daal::algorithms::preview::subgraph_isomorphism;
namespace interop = dal::backend::interop;

template <typename Float, daal::CpuType Cpu>
using daal_subgraph_isomorphism_vf3lp_dense_kernel_t = daal_subgraph_isomorphism::internal::
    preview::subgraph_isomorphismBatchKernel<daal_subgraph_isomorphism::vf3lpDense, Float, Cpu>;

template <typename Float, typename Task>
static graph_matching_result<Task> call_daal_kernel(const context_cpu& ctx,
                                                    const descriptor_t& desc,
                                                    const table& data,
                                                    const table& initial_centroids) {
    const int64_t row_count = data.get_row_count();
    const int64_t column_count = data.get_column_count();

    const int64_t cluster_count = desc.get_cluster_count();
    const int64_t max_iteration_count = desc.get_max_iteration_count();
    const double accuracy_threshold = desc.get_accuracy_threshold();

    daal_subgraph_isomorphism::Parameter par(
        dal::detail::integral_cast<std::size_t>(cluster_count),
        dal::detail::integral_cast<std::size_t>(max_iteration_count));
    par.accuracyThreshold = accuracy_threshold;

    auto arr_data = row_accessor<const Float>{ data }.pull();
    const auto daal_data =
        interop::convert_to_daal_homogen_table(arr_data, row_count, column_count);

    auto new_initial_centroids = initial_centroids;
    if (!new_initial_centroids.has_data()) {
        daal_subgraph_isomorphism_init::Parameter par(
            dal::detail::integral_cast<std::size_t>(cluster_count));

        const size_t init_len_input = 1;
        daal::data_management::NumericTable* init_input[init_len_input] = { daal_data.get() };

        auto daal_centroids =
            interop::allocate_daal_homogen_table<Float>(cluster_count, column_count);
        const size_t init_len_output = 1;
        daal::data_management::NumericTable* init_output[init_len_output] = {
            daal_centroids.get()
        };

        interop::status_to_exception(
            interop::call_daal_kernel<Float,
                                      daal_subgraph_isomorphism_init_plus_plus_dense_kernel_t>(
                ctx,
                init_len_input,
                init_input,
                init_len_output,
                init_output,
                &par,
                *(par.engine)));

        new_initial_centroids = interop::convert_from_daal_homogen_table<Float>(daal_centroids);
    }

    auto arr_initial_centroids = row_accessor<const Float>{ new_initial_centroids }.pull();

    dal::detail::check_mul_overflow(cluster_count, column_count);
    array<Float> arr_centroids = array<Float>::empty(cluster_count * column_count);
    array<int> arr_labels = array<int>::empty(row_count);
    array<Float> arr_objective_function_value = array<Float>::empty(1);
    array<int> arr_iteration_count = array<int>::empty(1);

    const auto daal_initial_centroids =
        interop::convert_to_daal_homogen_table(arr_initial_centroids,
                                               new_initial_centroids.get_row_count(),
                                               new_initial_centroids.get_column_count());
    const auto daal_centroids =
        interop::convert_to_daal_homogen_table(arr_centroids, cluster_count, column_count);
    const auto daal_labels = interop::convert_to_daal_homogen_table(arr_labels, row_count, 1);
    const auto daal_objective_function_value =
        interop::convert_to_daal_homogen_table(arr_objective_function_value, 1, 1);
    const auto daal_iteration_count =
        interop::convert_to_daal_homogen_table(arr_iteration_count, 1, 1);

    daal::data_management::NumericTable* input[2] = { daal_data.get(),
                                                      daal_initial_centroids.get() };

    daal::data_management::NumericTable* output[4] = { daal_centroids.get(),
                                                       daal_labels.get(),
                                                       daal_objective_function_value.get(),
                                                       daal_iteration_count.get() };

    interop::status_to_exception(
        interop::call_daal_kernel<Float, daal_subgraph_isomorphism_vf3lp_dense_kernel_t>(ctx,
                                                                                         input,
                                                                                         output,
                                                                                         &par));

    return graph_matching_result<Task>()
        .set_labels(dal::detail::homogen_table_builder{}.reset(arr_labels, row_count, 1).build())
        .set_iteration_count(static_cast<std::int64_t>(arr_iteration_count[0]))
        .set_objective_function_value(static_cast<double>(arr_objective_function_value[0]));
}

template <typename Float, typename Task>
static graph_matching_result<Task> graph_matching(const context_cpu& ctx,
                                                  const descriptor_t& desc,
                                                  const graph_matching_input<Task>& input) {
    return call_daal_kernel<Float, Task>(ctx,
                                         desc,
                                         input.get_data(),
                                         input.get_initial_centroids());
}

template <typename Float>
struct graph_matching_kernel_cpu<Float, method::vf3lp_dense, task::compute> {
    graph_matching_result<task::compute> operator()(
        const context_cpu& ctx,
        const descriptor_t& desc,
        const graph_matching_input<task::compute>& input) const {
        return graph_matching<Float, task::compute>(ctx, desc, input);
    }
};

template struct graph_matching_kernel_cpu<float, method::vf3lp_dense, task::compute>;
template struct graph_matching_kernel_cpu<double, method::vf3lp_dense, task::compute>;

} // namespace oneapi::dal::preview::subgraph_isomorphism::backend
