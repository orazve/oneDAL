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

NotImplementedException::NotImplementedException(const std::string& feature_name) :
  std::runtime_error(feature_name + " is not implemented") {
}

NotAvalibleDevice::NotAvalibleDevice(const std::string& feature_name) :
  std::runtime_error(feature_name + " is not implemented") {
}

NotAvalibleNumericTable::NotAvalibleNumericTable(const std::string& feature_name) :
  std::runtime_error(feature_name + " is not implemented") {
}

CannotFindObjectError::CannotFindObjectError(const std::string& object_name) :
  std::runtime_error("Cannot find object {" + object_name + "} in the registry") {
}

} // namespace dalbench
