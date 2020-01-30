/** file statistics.cpp */
/*******************************************************************************
* Copyright 2019-2020 Intel Corporation
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

#include "statistics.hpp"

#include <algorithm>

namespace dalbench {
namespace statistics {

double box_filter(const std::vector<double>& times) {
  const double left  = 0.25;
  const double right = 0.75;

  const size_t n                   = times.size();
  std::vector<double> sorted_times = times;
  std::sort(sorted_times.begin(), sorted_times.end());

  const double Q1 = sorted_times[size_t(n * left)];
  const double Q3 = sorted_times[size_t(n * right)];

  const double IQ = Q3 - Q1;

  const double lower = Q1 - 1.5 * IQ;
  const double upper = Q3 + 1.5 * IQ;

  double sum   = 0.0;
  double count = 0.0;

  for (int i = 0; i < n; i++) {
    const double timei = sorted_times[i];
    if ((lower <= timei) && (timei <= upper)) {
      count++;
      sum += timei;
    }
  }

  return count != 0.0 ? sum / count : count;
}

double first_iteration(const std::vector<double>& times) {
  return times[0];
}

} // namespace statistics
} // namespace dalbench
