#include "daalbench/engine.hpp"
#include "daalbench/datasets/utils.hpp"

namespace daalbench {
namespace datasets {

class DATASET(gdb9_cm) {
  Dataset Load() override {
    return DatasetFromCSV()
      .PathToTrain( Workload("melting-predict").PathToDataset("GDB9_CM_train.csv") )
      .PathToTest ( Workload("melting-predict").PathToDataset("GDB9_CM_test.csv")  )
      .FeaturesNumber(841)
      .Regression()
      .Load();
  }
};

} // namespace datasets
} // namespace daalbench
