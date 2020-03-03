/** file df_classification_params.hpp */
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

#ifndef DALBENCH_DECISION_FOREST_CLASSIFICATION_PARAMS_HPP_
#define DALBENCH_DECISION_FOREST_CLASSIFICATION_PARAMS_HPP_

#include "fixture.hpp"

namespace dalbench {

template <typename FPType>
class GetterParamsForestCls {
public:
  using nTrees       = size_t;
  using maxTreeDepth = size_t;

  struct Params : public CommonAlgorithmParams {
    Params(const DatasetName& dataset_name,
           const NumericTableType numeric_table_type,
           const size_t num_trees,
           const size_t max_tree_depth)
        : CommonAlgorithmParams(dataset_name, numeric_table_type),
          num_trees(num_trees),
          max_tree_depth(max_tree_depth) {}

    size_t num_trees;
    size_t max_tree_depth;
  };

  GetterParamsForestCls(const Params& params) : params(params) {}

  static DictionaryParams<Params> get_params() {
    return {
      { "Susy:4.5M;nT:10;D:5",
        Params(DatasetName("SUSY"), TableType(SyclHomogen, FPType), nTrees(10), maxTreeDepth(5)) },
      { "Mnist:60K;nT:100;D:10",
        Params(DatasetName("mnist"),
               TableType(SyclHomogen, FPType),
               nTrees(100),
               maxTreeDepth(10)) },
      { "Hepmass:150K;nT:50;D:15",
        Params(DatasetName("hepmass_small_classifier"),
               TableType(SyclHomogen, FPType),
               nTrees(50),
               maxTreeDepth(15)) }

    };
  }

  Params params;
};

} // namespace dalbench

#endif // DALBENCH_DECISION_FOREST_CLASSIFICATION_PARAMS_HPP_
