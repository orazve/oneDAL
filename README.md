# oneDAL Benchmarks
[![Build Status](https://dev.azure.com/daal/onedal-benchmarks/_apis/build/status/otcshare.onedal-benchmarks?branchName=master)](https://dev.azure.com/daal/onedal-benchmarks/_build/latest?definitionId=4&branchName=master) ![License](https://img.shields.io/github/license/intel/daal.svg)

The repo contains benchmarks for oneDAL algorithms. This software use Google Benchmark library under the hood and generate a report of performance metrics in human readable view.

## Benchmarks List

| **Category**                 | **Algorithm**                       | **CPU Batch** | **CPU Online** | **GPU Batch** | **GPU Online** |
| :---                         | :---                                | :---:         | :---:          | :---:         | :---:          |
| **Classification**           | Logistic Regression SGD             | &#x2611;      | **-**          | &#x2611;      | **-**          |
|                              | kNN Brute Force                     | **-**         | **-**          | &#x2611;      | **-**          |
| **Clustering**               | K-Means                             | &#x2611;      | **-**          | &#x2611;      | **-**          |
| **Dimensionality reduction** | Principal Components Analysis (PCA) | &#x2611;      | **-**          | &#x2611;      | **-**          |
| **Regression**               | Linear Regression                   | &#x2611;      | **-**          | &#x2611;      | **-**          |
| **Statistics**               | Covariance                          | &#x2611;      | &#x2611;       | &#x2611;      | &#x2611;       |
|                              | Moments of Low Order                | &#x2611;      | &#x2611;       | &#x2611;      | &#x2611;       |

## How to Build and Run oneDAL Benchmarks

### Setting Up Your Build Environment

To build and run `oneDAL benchmarks`, ensure the following dependencies are met:

* [CMake](https://cmake.org/) (>=3.6)
* [Intel(R) oneAPI Data Analytics Library](https://github.com/intel/daal)
* [Intel(R) oneAPI DPC++ Compiler](https://github.com/intel/llvm)

Information about how to get DPC++ Compiler and oneDAL packages can be found [here](https://software.intel.com/en-us/oneapi/onedal#oneapi-base-toolkit-download-popup-1).

Once dependencies are present, follow the steps below:

### Cloning Repository

Make sure that --recurse-submodules is used when clone the repository. This will fetch Google Benchmark as git submodule, which is required for building the project.

```bash
# Check out the onedal-benchmarks along with Google Benchmark as a submodule.
$ git clone --recurse-submodules https://github.com/otcshare/onedal-benchmarks
```

### Build Process

Go to the root directory of `onedal-benchmarks` and export the following enviromental variables:

```bash
# Go to the onedal-benchmarks directory
$ cd onedal-benchmarks
# Export DPCPP_ROOT
export DPCPP_ROOT=<install_dir>
# Export DAALROOT
export DAALROOT=<install_dir>/daal/latest
# Export DATASETSROOT
export DATASETSROOT=<current_dir>
```

<current_dir> is the root directory of `onedal-benchmarks` which was cloned.

<install_dir> is the directory in which Intel(R) oneAPI Base Toolkit was installed.

Also, `DAALROOT` and `DPCPP_ROOT` can be exported via `vars.sh` and `setvars.sh` scripts, respectively, from Intel(R) oneAPI Base Toolkit.

Build benchmarks:

```bash
# Make a build directory to place the build output.
mkdir _build && cd _build
# Generate a Makefile with cmake.
# Use cmake -G <generator> to generate a different file type.
cmake ../
# Also it may build with debug info
# $ cmake -DCMAKE_BUILD_TYPE=Debug ../
# Build the benchmarks.
# Use make -j<number_of_parallel_jobs> to speed up the build process, e.g. make -j8.
make -j install
```

### Running Benchmarks

#### Download Datasets

Before running benchmarks it's necessary to download datasets (if you have no it) which will be used with them. To download datasets `Python` is used whose version should be >=3.7.

Note, it's required as dependencies for `Python` the following [modules](requirements.txt) are used. To install modules required use the following command:

```bash
# Install modules required
$ pip install -r requirements.txt
```

Run scripts:

```bash
# Go to the workloads directory
$ cd ../workloads
# Run load_datasets.py script
$ python load_datasets.py
```

By default all datasets which are used with benchmarks will be downloaded. To see which datasets are available run `load_datasets.py` script with `-l` or `--list` option. To see help message run script with `-h` or `--help` option. Also, it's possible to download the one of datasets by the concrete loader. These loaders are located in corresponding folders of `workloads` directory.

```bash
# Example of higgs dataset downloading
$ python higgs_loader.py
```

#### Run Benchmarks

```bash
# Go to the build directory
$ cd ../_build
# Run becnhmarks for oneDAL algorithms
$ ./Benchmark
```

#### Benchmark Output

Below benchmark output is presented for `Logistic Regression SGD` algorithm with the following parameters:

- device at which algorithm is run: GPU
- data type of the algorithm: float
- repetitions count of the benchmark (algorithm): 5
- dataset: Susy with observations count is equal to 4.5 M

```bash
-------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                           Time        CPU  Iterations UserCounters...
-------------------------------------------------------------------------------------------------------------------------------
LogregTrainSGD/GpuDevice/float/Susy:4.5M/iterations:1/repeats:5_mean            29133 ms   20809 ms           5 applyBeta=4.03009k applyGradient=24.1492k buildProgram=140.492 makeStep=104.176 sigmoids=137.736 subVectors=363.964
LogregTrainSGD/GpuDevice/float/Susy:4.5M/iterations:1/repeats:5_median          28631 ms   20419 ms           5 applyBeta=3.81528k applyGradient=24.0848k buildProgram=0.502345 makeStep=36.5954 sigmoids=65.0416 subVectors=295.365
LogregTrainSGD/GpuDevice/float/Susy:4.5M/iterations:1/repeats:5_stddev           1129 ms     903 ms           5 applyBeta=502.621 applyGradient=143.781 buildProgram=313.049 makeStep=150.985 sigmoids=162.632 subVectors=159.857
LogregTrainSGD/GpuDevice/float/Susy:4.5M/iterations:1/repeats:5_box_filter      28628 ms   20406 ms           5 applyBeta=3.8054k applyGradient=24.0854k buildProgram=0.492648 makeStep=36.654 sigmoids=65.0047 subVectors=292.531
LogregTrainSGD/GpuDevice/float/Susy:4.5M/iterations:1/repeats:5_first_iteration 31152 ms   22423 ms           5 applyBeta=4.92885k applyGradient=24.4044k buildProgram=700.492 makeStep=374.263 sigmoids=428.661 subVectors=649.693
```

where:

- `Time` column devotes wall clock time
- `CPU` column devotes CPU time
- `Iterations` column devotes repetitions count of the benchmark (algorithm)
- `UserCounters` column devotes time of the algorithm kernels

Currently, the following time statistics are supported: `mean`, `median`, `stddev`, `box filter` and `first iteration`.

### Optional Possibilities of Running

```bash
# Get help
$ ./Benchmark --help
# Get full list of tests
$ ./Benchmark --benchmark_list_tests
# Select the concrete algorithm for all devices
$ ./Benchmark --benchmark_filter=PCA.*
# Select the concrete device for all algorithms
$ ./Benchmark --benchmark_filter=.*GpuDevice.*
# Select the concrete algorithm & device
$ ./Benchmark --benchmark_filter=PCA.*GpuDevice.*
# Save the result in JSON file
$ ./Benchmark --benchmark_out=result.json --benchmark_out_format=json
```

## Custom Build Options

`onedal-benchmarks` cmake has the following configurable flags available:

| **Flag**                      | **Possible Values**  | **Default Value** | **Behavior**                      |
| :---                          | :---:                | :---:             | :---                              |
| KERNEL_PROFILER<sup>[1]</sup> | [ON, OFF]            | OFF               | Enable/disable kernels profiling. |
| DAL_USE_DPCPP_INTERFACES      | [ON, OFF]            | ON                | Enable/disable DPCPP interfaces of oneDAL. Traditional interfaces of oneDAL will be used when flag is set to [OFF]. |
| DAL_USE_PARALLEL              | [ON, OFF]            | ON                | Enable/disable parallel version of oneDAL. Sequential version of oneDAL will be used when flag is set to [OFF]. |
| DAL_USE_STATIC                | [ON, OFF]            | ON                | Enable/disable static linking of oneDAL. Dynamic linking of oneDAL will be used when flag is set to [OFF]. |

<sup>[1]</sup> Kernels profiling for algorithms is available starting from oneDAL beta04 version.

## Command Line Options for oneDAL Benchmarks

The most useful options for us are:

| **Option**                           | **Parameter**              | **Description**                                   |
| :------                              | :-------                   | :-----------                                      |
| `--help`                             |                            | Show help message.                                |
| `--benchmark_list_tests`             |                            | Display the names of all the registered benchmarks and abort (one line per tuple of parameters). |
| `--benchmark_filter`                 | regex                      | Run only the benchmark which name contains a substring that matches the given regex. |
| `--benchmark_repetitions`            | repetitions count          | This option doesn't supported at the moment. By default, repetitions count is equal to 5 for `onedal-benchmarks` |
| `--benchmark_format`                 | [console, json, csv]       | Specify the format of the standard output.        |
| `--benchmark_out`                    | file path                  | Specify a file where to write the report.         |
| `--benchmark_out_format`             | [console, json, csv]       | Specify the format of the file output.            |
| `--v`                                | verbosity                  | Verbose level of the benchmark library.           |

## Licence

`oneDAL benchmarks` is licensed under [Apache License Version 2.0](http://www.apache.org/licenses/LICENSE-2.0). This software includes the following third-party components:

* [CMake](https://cmake.org/)
* [Google Benchmark](https://github.com/google/benchmark)

which are distributed under licences specified in the [third-party-programs.txt](third-party-programs.txt) file.
