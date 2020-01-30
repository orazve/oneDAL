/** file registry.hpp */
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

#ifndef DALBENCH_INCLUDE_REGISTRY_HPP_
#define DALBENCH_INCLUDE_REGISTRY_HPP_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "error_types.hpp"

namespace dalbench {

template <typename T>
class ObjectCreator {
public:
  virtual ~ObjectCreator()  = default;
  virtual T* create() const = 0;

  std::shared_ptr<T> create_shared() const {
    return std::shared_ptr<T>(create());
  }
};

template <typename T, typename U>
class DefaultObjectCreator : public ObjectCreator<T> {
  virtual U* create() const override {
    return new U();
  }
};

template <typename T>
class ObjectRegistry {
public:
  ObjectRegistry(ObjectRegistry&) = delete;
  ObjectRegistry& operator=(ObjectRegistry&) = delete;

  virtual ~ObjectRegistry() {
    for (const auto& pair : creators_) {
      delete pair.second;
    }
  }

  std::vector<std::string> names() const {
    std::vector<std::string> names;
    names.reserve(creators_.size());

    for (const auto& pair : creators_) {
      names.push_back(pair.first);
    }

    return names;
  }

  T* create(const std::string& name) {
    return get_creator(name)->create();
  }

  std::shared_ptr<T> create_shared(const std::string& name) {
    return get_creator(name)->create_shared();
  }

protected:
  ObjectRegistry() = default;

  int register_internal(const char* name, const ObjectCreator<T>* creator) {
    auto result = creators_.emplace(name, creator);
    return result.second ? 0 : -1;
  }

  const ObjectCreator<T>* get_creator(const std::string& name) {
    const auto it = creators_.find(name);
    if (it == creators_.end()) {
      throw CannotFindObject("Cannot find object {" + name + "} in the registry");
    }
    return it->second;
  }

private:
  std::map<std::string, const ObjectCreator<T>*> creators_;
};

template <typename T>
class DefaultObjectRegistry : public ObjectRegistry<T> {
public:
  static DefaultObjectRegistry<T>& instance() {
    static DefaultObjectRegistry<T> registry;
    return registry;
  }

  template <typename U>
  static int sign_up(const char* name) {
    auto creator = new DefaultObjectCreator<T, U>();
    return instance().register_internal(name, creator);
  }
};

template <typename T>
inline std::shared_ptr<T> create_registered(const std::string& name) {
  return DefaultObjectRegistry<T>::instance().create_shared(name);
}

#define REGISTER_DATASET(name, type)                             \
  static const volatile int __dataset_##type##_creator_dummy__ = \
    DefaultObjectRegistry<DatasetLoader>::sign_up<type>(name);

} // namespace dalbench

#endif // DALBENCH_INCLUDE_REGISTRY_HPP_
