#include "daalbench/engine.hpp"
#include "daalbench/datasets/utils.hpp"

namespace daalbench {
namespace datasets {

class DATASET(airline) {
  Dataset Load() override {
    return DatasetFromCSV()
      .PathToFull( Workload("airline").PathToDataset("airline.csv") )
      .FeaturesNumber(8)
      .ResponsesNumber(2)
      .Load();
  }
};

class DATASET(plasticc) {
  Dataset Load() override {
    return DatasetFromCSV()
      .PathToTrain( Workload("plasticc").PathToDataset("PLAsTiCC_train.csv") )
      .PathToTest ( Workload("plasticc").PathToDataset("PLAsTiCC_test.csv")  )
      .FeaturesNumber(69)
      .ResponsesNumber(14)
      .Load();
  }
};

class DATASET(mnist) {
  Dataset Load() override {
    return DatasetFromCSV()
      .PathToFull ( Workload("mnist").PathToDataset("mnist.csv")       )
      .PathToTrain( Workload("mnist").PathToDataset("mnist_train.csv") )
      .PathToTest ( Workload("mnist").PathToDataset("mnist_test.csv")  )
      .FeaturesNumber(784)
      .ResponsesNumber(10)
      .Load();
  }
};

} // namespace datasets
} // namespace daalbench
