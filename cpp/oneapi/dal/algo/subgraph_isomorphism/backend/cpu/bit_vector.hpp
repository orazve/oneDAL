/*******************************************************************************
* Copyright 2021 Intel Corporation
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

#include "oneapi/dal/common.hpp"
#include "oneapi/dal/algo/subgraph_isomorphism/backend/cpu/adapt.hpp"
#include "oneapi/dal/algo/subgraph_isomorphism/backend/cpu/inner_alloc.hpp"
#include "oneapi/dal/algo/subgraph_isomorphism/backend/cpu/graph_status.hpp"
#include "oneapi/dal/backend/dispatcher.hpp"
#include "oneapi/dal/detail/common.hpp"

namespace oneapi::dal::preview::subgraph_isomorphism::backend {

template <typename Cpu>
class bit_vector {
public:
    // precomputed count of ones in a number from 0 to 255
    // e.g. bit_set_table[2] = 1, because of 2 is 0x00000010
    // e.g. bit_set_table[3] = 7, because of 7 is 0x00000111
    static constexpr std::uint8_t bit_set_table[256] = {
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3,
        4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4,
        4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4,
        5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5,
        4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2,
        3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5,
        5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4,
        5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6,
        4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
    };

    static constexpr std::int64_t byte(std::int64_t x) {
        return x >> 3;
    };

    static constexpr std::uint8_t bit(std::int64_t x) {
        return 1 << (x & 7);
    };

    static constexpr std::int64_t bit_vector_size(std::int64_t vertex_count) {
        return -~(vertex_count >> 3); /* (vertex_count / 8) + 1 */
    };

    static constexpr std::uint8_t power_of_two(const std::uint8_t bit_val) {
        return 31 - ONEDAL_lzcnt_u32(bit_val);
    };

    static void split_by_registers(const std::int64_t vector_size,
                                   std::int64_t* registers,
                                   const register_size max_size);

    bit_vector(const inner_alloc allocator);
    bit_vector(bit_vector<Cpu>& bvec);
    bit_vector(const std::int64_t vector_size, inner_alloc allocator);
    bit_vector(const std::int64_t vector_size, const std::uint8_t byte_val, inner_alloc allocator);
    bit_vector(const std::int64_t vector_size, std::uint8_t* pvector, inner_alloc allocator);
    ~bit_vector();
    graph_status unset_bit(std::int64_t vertex);
    graph_status set_bit(std::int64_t vertex);
    void set(const std::uint8_t byte_val = 0x0);
    std::uint8_t* get_vector_pointer() const;
    std::int64_t size() const;
    std::int64_t popcount() const;

    bit_vector<Cpu>& operator&=(bit_vector<Cpu>& a);
    bit_vector<Cpu>& operator|=(bit_vector<Cpu>& a);
    bit_vector<Cpu>& operator^=(bit_vector<Cpu>& a);
    bit_vector<Cpu>& operator=(const bit_vector<Cpu>& a);
    bit_vector<Cpu>& operator~();
    bit_vector<Cpu>& operator&=(const std::uint8_t* pa);
    bit_vector<Cpu>& operator|=(const std::uint8_t* pa);
    bit_vector<Cpu>& operator^=(const std::uint8_t* pa);
    bit_vector<Cpu>& operator=(const std::uint8_t* pa);

    bit_vector<Cpu>& or_equal(const std::int64_t* bit_index, const std::int64_t list_size);
    bit_vector<Cpu>& and_equal(const std::int64_t* bit_index,
                               const std::int64_t list_size,
                               std::int64_t* tmp_array = nullptr,
                               const std::int64_t tmp_size = 0);

    bit_vector(bit_vector<Cpu>&& a);

    bit_vector<Cpu>& andn(const std::uint8_t* pa);
    bit_vector<Cpu>& andn(bit_vector<Cpu>& a);
    bit_vector<Cpu>& orn(const std::uint8_t* pa);
    bit_vector<Cpu>& orn(bit_vector<Cpu>& a);

    std::int64_t min_index() const;
    std::int64_t max_index() const;

    graph_status get_vertices_ids(std::int64_t* vertices_ids);
    bool test_bit(const std::int64_t vertex);

    static graph_status set(const std::int64_t vector_size,
                            std::uint8_t* result_vector,
                            const std::uint8_t byte_val = 0);
    static graph_status set(const std::int64_t vector_size,
                            std::uint8_t* result_vector,
                            const std::uint8_t* vector);
    static graph_status unset_bit(std::uint8_t* result_vector, const std::int64_t vertex);
    static graph_status set_bit(std::uint8_t* result_vector, const std::int64_t vertex);
    static std::int64_t get_bit_index(const std::int64_t vector_size,
                                      const std::uint8_t* vector,
                                      const std::int64_t vertex);
    static std::int64_t popcount(const std::int64_t vector_size, const std::uint8_t* vector);
    static bool test_bit(const std::int64_t vector_size,
                         const std::uint8_t* vector,
                         const std::int64_t vertex);

private:
    inner_alloc allocator_;
    std::uint8_t* vector;
    std::int64_t n;
};

template <typename Cpu>
void or_equal(std::uint8_t* ONEAPI_RESTRICT vec,
              const std::uint8_t* ONEAPI_RESTRICT pa,
              std::int64_t size) {
    ONEDAL_IVDEP
    for (std::int64_t i = 0; i < size; i++) {
        vec[i] |= pa[i];
    }
}

template <typename Cpu>
void and_equal(std::uint8_t* ONEAPI_RESTRICT vec,
               const std::uint8_t* ONEAPI_RESTRICT pa,
               std::int64_t size) {
    ONEDAL_IVDEP
    for (std::int64_t i = 0; i < size; i++) {
        vec[i] &= pa[i];
    }
}

template <typename Cpu>
void inversion(std::uint8_t* ONEAPI_RESTRICT vec, std::int64_t size) {
    ONEDAL_IVDEP
    for (std::int64_t i = 0; i < size; i++) {
        vec[i] = ~vec[i];
    }
}

template <typename Cpu>
void or_equal(std::uint8_t* ONEAPI_RESTRICT vec,
              const std::int64_t* ONEAPI_RESTRICT bit_index,
              const std::int64_t list_size) {
    ONEDAL_IVDEP
    for (std::int64_t i = 0; i < list_size; i++) {
        vec[bit_vector<Cpu>::byte(bit_index[i])] |= bit_vector<Cpu>::bit(bit_index[i]);
    }
}

template <typename Cpu>
void set(std::uint8_t* ONEAPI_RESTRICT vec, std::int64_t size, const std::uint8_t byte_val = 0x0) {
    ONEDAL_VECTOR_ALWAYS
    for (std::int64_t i = 0; i < size; i++) {
        vec[i] = byte_val;
    }
}

template <typename Cpu>
void and_equal(std::uint8_t* ONEAPI_RESTRICT vec,
               const std::int64_t* ONEAPI_RESTRICT bit_index,
               const std::int64_t bit_size,
               const std::int64_t list_size,
               std::int64_t* ONEAPI_RESTRICT tmp_array = nullptr,
               const std::int64_t tmp_size = 0) {
    std::int64_t counter = 0;
    ONEDAL_IVDEP
    for (std::int64_t i = 0; i < list_size; i++) {
        tmp_array[counter] = bit_index[i];
        counter += bit_vector<Cpu>::bit_set_table[vec[bit_vector<Cpu>::byte(bit_index[i])] &
                                                  bit_vector<Cpu>::bit(bit_index[i])];
    }

    set<Cpu>(vec, bit_size, 0x0);

    ONEDAL_IVDEP
    for (std::int64_t i = 0; i < counter; i++) {
        vec[bit_vector<Cpu>::byte(tmp_array[i])] |= bit_vector<Cpu>::bit(tmp_array[i]);
    }
}

template <typename Cpu>
graph_status bit_vector<Cpu>::set(const std::int64_t vector_size,
                                  std::uint8_t* result_vector,
                                  const std::uint8_t* vector) {
    if (result_vector == nullptr || vector == nullptr) {
        return bad_allocation;
    }

    for (std::int64_t i = 0; i < vector_size; i++) {
        result_vector[i] = vector[i];
    }

    /*
      TODO improvements  use register and sets functions
    */

    return ok;
}

template <typename Cpu>
graph_status bit_vector<Cpu>::set(const std::int64_t vector_size,
                                  std::uint8_t* result_vector,
                                  const std::uint8_t byte_val) {
    if (result_vector == nullptr) {
        return bad_allocation;
    }

    for (std::int64_t i = 0; i < vector_size; i++) {
        result_vector[i] = byte_val;
    }

    /*
        TODO improvements  use register and sets functions
    */

    return ok;
}

template <typename Cpu>
void bit_vector<Cpu>::set(const std::uint8_t byte_val) {
    const std::int64_t nn = n;
    ONEDAL_VECTOR_ALWAYS
    for (std::int64_t i = 0; i < nn; i++) {
        vector[i] = byte_val;
    }
}

template <typename Cpu>
graph_status bit_vector<Cpu>::set_bit(std::uint8_t* result_vector, const std::int64_t vertex) {
    if (result_vector == nullptr) {
        return bad_allocation;
    }
    result_vector[byte(vertex)] |= bit(vertex);
    return ok;
}

template <typename Cpu>
graph_status bit_vector<Cpu>::unset_bit(std::uint8_t* result_vector, const std::int64_t vertex) {
    if (result_vector == nullptr) {
        return bad_allocation;
    }
    result_vector[byte(vertex)] &= ~bit(vertex);
    return ok;
}

template <typename Cpu>
bit_vector<Cpu>::bit_vector(const inner_alloc allocator) : allocator_(allocator) {
    vector = nullptr;
    n = 0;
}

template <typename Cpu>
bit_vector<Cpu>::bit_vector(bit_vector<Cpu>& bvec)
        : allocator_(bvec.allocator_.get_byte_allocator()) {
    n = bvec.n;
    vector = allocator_.allocate<std::uint8_t>(n);
    this->set(n, vector, bvec.vector);
}

template <typename Cpu>
bit_vector<Cpu>::bit_vector(const std::int64_t vector_size, inner_alloc allocator)
        : bit_vector(allocator) {
    n = vector_size;
    vector = allocator_.allocate<std::uint8_t>(n);
    this->set(n, vector);
}

template <typename Cpu>
bit_vector<Cpu>::bit_vector(const std::int64_t vector_size,
                            const std::uint8_t byte_val,
                            inner_alloc allocator)
        : allocator_(allocator) {
    n = vector_size;
    vector = allocator_.allocate<std::uint8_t>(n);
    this->set(n, vector, byte_val);
}

template <typename Cpu>
bit_vector<Cpu>::bit_vector(bit_vector<Cpu>&& a)
        : allocator_(a.allocator_.get_byte_allocator()),
          vector(a.vector),
          n(a.n) {
    a.n = 0;
    a.vector = nullptr;
}

template <typename Cpu>
bit_vector<Cpu>::~bit_vector() {
    if (vector != nullptr) {
        allocator_.deallocate<std::uint8_t>(vector, n);
    }
}

template <typename Cpu>
bit_vector<Cpu>::bit_vector(const std::int64_t vector_size,
                            std::uint8_t* pvector,
                            inner_alloc allocator)
        : allocator_(allocator),
          n(vector_size),
          vector(pvector) {}

template <typename Cpu>
graph_status bit_vector<Cpu>::unset_bit(std::int64_t vertex) {
    return unset_bit(vector, vertex);
}

template <typename Cpu>
graph_status bit_vector<Cpu>::set_bit(std::int64_t vertex) {
    return set_bit(vector, vertex);
}

template <typename Cpu>
std::uint8_t* bit_vector<Cpu>::get_vector_pointer() const {
    return vector;
}

template <typename Cpu>
std::int64_t bit_vector<Cpu>::size() const {
    return n;
}

template <typename Cpu>
bit_vector<Cpu>& bit_vector<Cpu>::operator&=(bit_vector<Cpu>& a) {
    if (n <= a.size()) {
        const std::uint8_t* pa = a.get_vector_pointer();
        for (std::int64_t i = 0; i < n; i++) {
            vector[i] &= pa[i];
        }
    }
    return *this;
}

template <typename Cpu>
bit_vector<Cpu>& bit_vector<Cpu>::operator|=(bit_vector<Cpu>& a) {
    if (n <= a.size()) {
        const std::uint8_t* pa = a.get_vector_pointer();
        for (std::int64_t i = 0; i < n; i++) {
            vector[i] |= pa[i];
        }
    }
    return *this;
}

template <typename Cpu>
bit_vector<Cpu>& bit_vector<Cpu>::operator^=(bit_vector<Cpu>& a) {
    if (n <= a.size()) {
        std::uint8_t* pa = a.get_vector_pointer();
        for (std::int64_t i = 0; i < n; i++) {
            vector[i] ^= pa[i];
        }
    }
    return *this;
}

template <typename Cpu>
bit_vector<Cpu>& bit_vector<Cpu>::operator=(const bit_vector<Cpu>& a) {
    if (n <= a.size()) {
        const std::uint8_t* pa = a.get_vector_pointer();
        for (std::int64_t i = 0; i < n; i++) {
            vector[i] = pa[i];
        }
    }
    return *this;
}

template <typename Cpu>
bit_vector<Cpu>& bit_vector<Cpu>::operator~() {
    ONEDAL_IVDEP
    for (std::int64_t i = 0; i < n; i++) {
        vector[i] = ~vector[i];
    }
    return *this;
}

template <typename Cpu>
bit_vector<Cpu>& bit_vector<Cpu>::operator&=(const std::uint8_t* ONEAPI_RESTRICT pa) {
    for (std::int64_t i = 0; i < n; i++) {
        vector[i] &= pa[i];
    }
    return *this;
}

template <typename Cpu>
bit_vector<Cpu>& bit_vector<Cpu>::operator|=(const std::uint8_t* ONEAPI_RESTRICT pa) {
    for (std::int64_t i = 0; i < n; i++) {
        vector[i] |= pa[i];
    }
    return *this;
}

template <typename Cpu>
bit_vector<Cpu>& bit_vector<Cpu>::operator^=(const std::uint8_t* pa) {
    for (std::int64_t i = 0; i < n; i++) {
        vector[i] ^= pa[i];
    }
    return *this;
}

template <typename Cpu>
bit_vector<Cpu>& bit_vector<Cpu>::operator=(const std::uint8_t* pa) {
    for (std::int64_t i = 0; i < n; i++) {
        vector[i] = pa[i];
    }
    return *this;
}

template <typename Cpu>
bit_vector<Cpu>& bit_vector<Cpu>::andn(const std::uint8_t* pa) {
    for (std::int64_t i = 0; i < n; i++) {
        vector[i] &= ~pa[i];
    }
    return *this;
}

template <typename Cpu>
bit_vector<Cpu>& bit_vector<Cpu>::andn(bit_vector<Cpu>& a) {
    if (n <= a.size()) {
        std::uint8_t* pa = a.get_vector_pointer();
        for (std::int64_t i = 0; i < n; i++) {
            vector[i] &= ~pa[i];
        }
    }
    return *this;
}

template <typename Cpu>
bit_vector<Cpu>& bit_vector<Cpu>::orn(const std::uint8_t* pa) {
    for (std::int64_t i = 0; i < n; i++) {
        vector[i] |= ~pa[i];
    }
    return *this;
}

template <typename Cpu>
bit_vector<Cpu>& bit_vector<Cpu>::orn(bit_vector<Cpu>& a) {
    if (n <= a.size()) {
        std::uint8_t* pa = a.get_vector_pointer();
        for (std::int64_t i = 0; i < n; i++) {
            vector[i] |= ~pa[i];
        }
    }
    return *this;
}

template <typename Cpu>
bit_vector<Cpu>& bit_vector<Cpu>::or_equal(const std::int64_t* bit_index,
                                           const std::int64_t list_size) {
    for (std::int64_t i = 0; i < list_size; i++) {
        vector[byte(bit_index[i])] |= bit(bit_index[i]);
    }
    return *this;
}

template <typename Cpu>
bit_vector<Cpu>& bit_vector<Cpu>::and_equal(const std::int64_t* bit_index,
                                            const std::int64_t list_size,
                                            std::int64_t* tmp_array,
                                            const std::int64_t tmp_size) {
    std::int64_t counter = 0;
    for (std::int64_t i = 0; i < list_size; i++) {
        tmp_array[counter] = bit_index[i];
        counter += bit_set_table[vector[byte(bit_index[i])] & bit(bit_index[i])];
    }

    this->set(0x0);
    for (std::int64_t i = 0; i < counter; i++) {
        vector[byte(tmp_array[i])] |= bit(tmp_array[i]);
    }
    return *this;
}

template <typename Cpu>
graph_status bit_vector<Cpu>::get_vertices_ids(std::int64_t* vertices_ids) {
    std::int64_t vertex_iterator = 0;
    if (vertices_ids == nullptr) {
        return bad_arguments;
    }
    std::uint8_t current_byte = 0;
    std::uint8_t current_bit = 0;
    for (std::int64_t i = 0; i < n; i++) {
        current_byte = vector[i];
        while (current_byte > 0) {
            current_bit = current_byte & (-current_byte);
            current_byte &= ~current_bit;
            vertices_ids[vertex_iterator] = 8 * i + power_of_two(current_bit);
            vertex_iterator++;
        }
    }
    return static_cast<graph_status>(vertex_iterator);
}

template <typename Cpu>
bool bit_vector<Cpu>::test_bit(const std::int64_t vertex) {
    return vector[byte(vertex)] & bit(vertex);
}

template <typename Cpu>
bool bit_vector<Cpu>::test_bit(const std::int64_t vector_size,
                               const std::uint8_t* vector,
                               const std::int64_t vertex) {
    if (vertex / 8 > vector_size) {
        return false;
    }
    return vector[byte(vertex)] & bit(vertex);
}

template <typename Cpu>
std::int64_t bit_vector<Cpu>::get_bit_index(const std::int64_t vector_size,
                                            const std::uint8_t* vector,
                                            const std::int64_t vertex) {
    if (vertex / 8 > vector_size) {
        return bad_arguments;
    }

    std::int64_t nbyte = byte(vertex);
    std::uint8_t bit = vertex & 7;
    std::int64_t result = 0;

    for (std::int64_t i = 0; i < nbyte; i++)
        result += bit_vector<Cpu>::bit_set_table[vector[i]];

    std::uint8_t checkbyte = vector[nbyte];
    for (std::uint8_t i = 0; i < bit; i++) {
        result += checkbyte & 1;
        checkbyte >>= 1;
    }
    return result;
}

template <typename Cpu>
std::int64_t bit_vector<Cpu>::popcount(const std::int64_t vector_size, const std::uint8_t* vector) {
    if (vector == nullptr) {
        return bad_arguments;
    }

    std::int64_t result = 0;
    for (std::int64_t i = 0; i < vector_size; i++) {
        result += bit_vector<Cpu>::bit_set_table[vector[i]];
    }

    return result;
}

template <typename Cpu>
std::int64_t bit_vector<Cpu>::popcount() const {
    return popcount(n, vector);
}

template <typename Cpu>
void bit_vector<Cpu>::split_by_registers(const std::int64_t vector_size,
                                         std::int64_t* registers,
                                         const register_size max_size) {
    std::int64_t size = vector_size;
    for (std::int64_t i = power_of_two(max_size); i >= 0; i--) {
        registers[i] = size >> i;
        size -= registers[i] << i;
    }
}

template <typename Cpu>
std::int64_t bit_vector<Cpu>::min_index() const {
    std::int64_t result = 0;

    for (std::int64_t i = n; i > 0; i--) {
        if (vector[i] > 0) {
            result = power_of_two(vector[i]);
            result += (i << 3);
            break;
        }
    }
    return result;
}

template <typename Cpu>
std::int64_t bit_vector<Cpu>::max_index() const {
    std::int64_t result = 1 << n;

    for (std::int64_t i = 0; i < n; i++) {
        if (vector[i] > 0) {
            result = power_of_two(vector[i]);
            result += (i << 3);
            break;
        }
    }
    return result;
}

} // namespace oneapi::dal::preview::subgraph_isomorphism::backend
