/** file error_types.hpp */
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

#ifndef DALBENCH_INCLUDE_ERROR_TYPES_HPP_
#define DALBENCH_INCLUDE_ERROR_TYPES_HPP_

#include <stdexcept>
#include <string>

namespace dalbench {

class NotImplementedException : public std::runtime_error {
public:
  explicit NotImplementedException(const std::string& feature_name);
};

class NotAvalibleDevice : public std::runtime_error {
public:
  explicit NotAvalibleDevice(const std::string& feature_name);
};

class NotAvalibleNumericTable : public std::runtime_error {
public:
  explicit NotAvalibleNumericTable(const std::string& feature_name);
};

class CannotFindObjectError : public std::runtime_error {
public:
  explicit CannotFindObjectError(const std::string& object_name);
};

} // namespace dalbench

#endif // DALBENCH_INCLUDE_ERROR_TYPES_HPP_
