#include "daalbench/engine.hpp"
#include "daalbench/datasets/utils.hpp"

namespace daalbench {
namespace datasets {

class DATASET(sdss) {
  Dataset Load() override {
    return DatasetFromCSV()
      .PathToFull( Workload("sdss").PathToDataset("sdss.csv") )
      .FeaturesNumber(2)
      .ResponsesNumber(4)
      .Load();
  }
};

class DATASET(higgs_cluster_10M) {
  Dataset Load() override {
    return DatasetFromCSV()
      .PathToFull( Workload("higgs").PathToDataset("higgs_cluster_10M.csv") )
      .FeaturesNumber(28)
      .ResponsesNumber(100)
      .Unlabeled()
      .OnError("Make sure that HIGGS dataset is downloaded and extracted. "
               "To download dataset run `download.sh` located in HIGGS workload "
               "directory: " + Workload("higgs").Path() + ". Next, run `python "
               "extract_for_clustering.py`, find out more details in README.")
      .Load();
  }
};

class DATASET(gdb9_cm_cluster) {
  Dataset Load() override {
    return DatasetFromCSV()
      .PathToTrain( Workload("melting-predict").PathToDataset("GDB9_CM_train.csv") )
      .PathToTest ( Workload("melting-predict").PathToDataset("GDB9_CM_test.csv")  )
      .FeaturesNumber(841)
      .ResponsesNumber(50)
      .Load();
  }
};

class DATASET(multi_event_cluster) {
  Dataset Load() override {
    return DatasetFromCSV()
      .PathToFull( Workload("multi_event_cluster").PathToDataset("event_datasets.csv") )
      .PathToIndex( Workload("multi_event_cluster").PathToDataset("event_index.csv") )
      .FeaturesNumber(2)
      .ResponsesNumber(6)
      .TriesNumber(300)
      .Unlabeled()
      .OnError("Make sure multi_events data set is in place.")
      .Load();
  }
};

} // namespace datasets
} // namespace daalbench
