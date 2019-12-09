/** file error_types.cpp */
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

#include "error_types.hpp"

namespace dalbench {

NotAvailableDevice::NotAvailableDevice(const std::string& error_message)
    : std::runtime_error(error_message) {
}

EmptyNumericTable::EmptyNumericTable(const std::string& error_message)
    : std::runtime_error(error_message) {
}

NotAvailableNumericTable::NotAvailableNumericTable(const std::string& error_message)
    : std::runtime_error(error_message) {
}

CannotFindObject::CannotFindObject(const std::string& error_message)
    : std::runtime_error(error_message) {
}

CannotOpenFile::CannotOpenFile(const std::string& error_message)
    : std::runtime_error(error_message) {
}

CannotLoadDataset::CannotLoadDataset(const std::string& error_message)
    : std::runtime_error(error_message) {
}

CannotReadCsv::CannotReadCsv(const std::string& error_message) : std::runtime_error(error_message) {
}

NotDefinedEnvironmentVariable::NotDefinedEnvironmentVariable(const std::string& error_message)
    : std::runtime_error(error_message) {
}

} // namespace dalbench
