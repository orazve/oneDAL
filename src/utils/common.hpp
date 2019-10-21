#include "benchmark/benchmark.h"
#include <set>
#include <vector>


namespace daalbench {

using DatasetParams = std::string;

template <typename algtype, typename fptype>
class Fixture : public ::benchmark::Fixture {
public:
    Fixture() : ::benchmark::Fixture()  {}
    Fixture() = delete;

    void SetUp(const ::benchmark::State& state) override {
        setParameters();
        setInput();
        // INIT DATA
        printf("SetUp\n");
    }

    void TearDown(const ::benchmark::State& state) override {
        // FREE DATA
        printf("TearDown\n");
    }

protected:
    void BenchmarkCase(::benchmark::State& state) {
        runBenchmark(state);
        // generateMetrics(state);
    }

    void runBenchmark(::benchmark::State& state) override {
        for (auto _ : state) {
            algorithm.compute();
        }
    }

    virtual void setInput() = 0;
    virtual void setParameters() = 0;

    const algtype& algorithm;
};

#define DAAL_BENCH_REGISTER(ParamsClass, BaseClass, BaseName, params)        \
  static internal::Registrar<ParamsClass, BaseClass> BENCHMARK_PRIVATE_NAME( \
    registrar)(params, #BaseClass "/" BaseName)



} // daalbench