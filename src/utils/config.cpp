/** file config.cpp */
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

#include "config.hpp"

#include "error_types.hpp"

namespace dalbench {

std::string get_env_var(const char* var_name) {
  const char* value = std::getenv(var_name);
  if (!value) {
    throw NotDefinedEnvironmentVariable("Environment variable " + std::string(var_name) +
                                        " undefined");
  }
  return std::string(value);
}

GlobalConfig::GlobalConfig() {
  root_path_ = get_env_var("DATASETSROOT");
}

const GlobalConfig& GlobalConfig::instance() {
  static const GlobalConfig config;
  return config;
}

const std::string GlobalConfig::root_path() const {
  return root_path_;
}

} // namespace dalbench
