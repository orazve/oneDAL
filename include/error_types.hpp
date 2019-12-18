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

class NotAvailableDevice : public std::runtime_error {
public:
    explicit NotAvailableDevice(const std::string& error_message);
};

class EmptyNumericTable : public std::runtime_error {
public:
    explicit EmptyNumericTable(const std::string& error_message);
};

class NotAvailableNumericTable : public std::runtime_error {
public:
    explicit NotAvailableNumericTable(const std::string& error_message);
};

class CannotFindObject : public std::runtime_error {
public:
    explicit CannotFindObject(const std::string& error_message);
};

class CannotOpenFile : public std::runtime_error {
public:
    explicit CannotOpenFile(const std::string& error_message);
};

class CannotLoadDataset : public std::runtime_error {
public:
    explicit CannotLoadDataset(const std::string& error_message);
};

class CannotReadCsv : public std::runtime_error {
public:
    explicit CannotReadCsv(const std::string& error_message);
};

class NotDefinedEnvironmentVariable : public std::runtime_error {
public:
    explicit NotDefinedEnvironmentVariable(const std::string& error_message);
};

} // namespace dalbench

#endif // DALBENCH_INCLUDE_ERROR_TYPES_HPP_
