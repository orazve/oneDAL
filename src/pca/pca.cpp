#include "benchmark/benchmark.h"
#include "common.hpp"
#include "daataset.hpp"
#include <vector>

namespace daalbench {

template <typename fptype>
struct Params {
    DatasetParams dataset_name;
    BatchParameter<fptype, correlationDense> params_alg;
};

template <typename fptype>
class PCA : public Fixture<pca::Batch<fptype>, fptype> {
public:
    PCA(const std::string& name, const Params& p) :
        Fixture<pca::Batch<fptype>, fptype>(), params(p) {
        this->SetName(name.c_str());
    }

protected:
    virtual void setParameters() {
        this->algorithm.parameter() = params.params_algorithms;
    }

    virtual void setInput() {
        auto dataset_loader = CreateRegistered<DatasetLoader>(params.dataset_name);
        auto dataset = dataset_loader->Load();

        auto X = dataset.Train().X();
        this->algorithm.input.set(pca::data, X);
    }

private:
    const Params& params;
};

std::vector<Params> getInputs() {
    std::vector<Params> out;
    Params p;

    // Datasets({ "airline", "plasticc", "mnist" })

    std::vector<std::string> datasets = { "airline", "plasticc", "mnist" };
    const std::vector<bool> is_deterministics = {true, false};
    const std::vector<size_t> n_components = {0, 100};

    for (auto& dataset : datasets) {
        p.dataset_name = dataset;
        for (auto& is_deterministic : is_deterministics) {
            p.params_alg.isDeterministic = is_deterministic;
            for (auto n_component : n_components) {
                p.params_alg.nComponents = n_component;
                out.push_back(p);
            }
        }
    }
    return out;
}

DAAL_BENCH_REGISTER(Params, PCA<float>, "blobs", getInputs());
DAAL_BENCH_REGISTER(Params, PCA<double>, "blobs", getInputs());

}  // end namespace daalbench