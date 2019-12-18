/** file service.hpp */
/*******************************************************************************
* Copyright 2019 Intel Corporation
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

#ifndef DALBENCH_INCLUDE_SERVICE_HPP_
#define DALBENCH_INCLUDE_SERVICE_HPP_

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace dalbench {

template <typename Delimeter>
inline std::string join(const std::vector<std::string>& chunks, const Delimeter& delimeter) {
  if (chunks.empty()) {
    return std::string();
  }

  std::stringstream stream;

  stream << chunks.front();
  for (size_t i = 1; i < chunks.size(); i++) {
    if (!chunks[i].empty()) {
      stream << delimeter << chunks[i];
    }
  }
  return stream.str();
}

inline std::string join_path(const std::vector<std::string>& chunks) {
  return join(chunks, '/');
}

inline std::string join_sentences(const std::vector<std::string>& chunks) {
  return join(chunks, ". ");
}

inline bool can_open_file(const std::string& path) {
  return std::ifstream(path, std::fstream::in).is_open();
}

} // namespace dalbench

#endif // DALBENCH_INCLUDE_SERVICE_HPP_
