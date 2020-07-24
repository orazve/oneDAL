/* file: data_source_el.cpp */
/*******************************************************************************
* Copyright 2014-2020 Intel Corporation
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

/*
!  Content:
!    C++ example of Cholesky decomposition
!******************************************************************************/

/**
 * <a name="DAAL-EXAMPLE-CPP-CHOLESKY_BATCH"></a>
 * \example cholesky_dense_batch.cpp
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <string>
#include <cassert>

#define DAAL_SUMMARY_STATISTICS_TYPE int32_t
#define DAAL_DATA_TYPE int32_t
#include "daal.h"
#include "service.h"
// #include "istrconv_daal_el.h"

#include "tbb/global_control.h"
#include "tbb/task_arena.h"

#define CR_INIT() auto t0 = std::chrono::high_resolution_clock::now(); auto t1 = std::chrono::high_resolution_clock::now();
#define CR_ST() t0 = std::chrono::high_resolution_clock::now();
#define CR_END(name) t1 = std::chrono::high_resolution_clock::now(); \
    { \
        std::chrono::duration<double> elapsed = t1 - t0; \
        std::cout << "Elapsed time " << #name <</*" ["<<tbb::task_arena::current_thread_index()<<"]: " <<*/ elapsed.count() << " s\n"; \
    }


using namespace std;
using namespace daal;
using namespace daal::algorithms;
using namespace daal::data_management;

#define NodeId_t int32_t

/* Input data set parameters */
// string datasetFileName = "../data/batch/cholesky.csv"; char delimeter = ',';
// string datasetFileName = "/nfs/inn/proj/numerics1/Users/orazvens/oneapi-onedal-orazve/oneDAL/examples/daal/data/batch/el10.el"; char delimeter = ' ';
string datasetFileName = "/nfs/inn/proj/numerics1/Users/orazvens/share/USA-road-d.USA.el"; char delimeter = ' ';

template<class node_id_t = int32_t>
std::vector<node_id_t> read_edge_list(const std::string &path)
{
    std::ifstream file(path);
    assert(file.is_open());
    std::vector<node_id_t> vertices;
    node_id_t source_vertex = 0;
    node_id_t destination_vertex = 0;
    while(file >> source_vertex >> destination_vertex) {
        vertices.push_back(source_vertex);
        vertices.push_back(destination_vertex);
    }
    file.close();
    return vertices;
}

template<class node_id_t = int32_t>
std::vector<node_id_t> read_edge_list_fpk_while(const std::string &path)
{
    std::ifstream file(path);
    assert(file.is_open());
    std::vector<node_id_t> vertices;
    char source_vertex[32], destination_vertex[32];
    while(file >> source_vertex >> destination_vertex) {
        vertices.push_back(daal::services::daal_string_to_int(&source_vertex[0], 0));
        vertices.push_back(daal::services::daal_string_to_int(&destination_vertex[0], 0));
    }
    file.close();
    return vertices;
}

template<class node_id_t = int32_t>
std::vector<node_id_t> read_edge_list_fpk(const std::string &path)
{
    std::ifstream file(path);
    std::vector<node_id_t> vertices;
    char line[80];
    char word[40];
    while ( file.getline(line, 80) ) {
        std::istringstream iss(line);
        while ( iss.getline( word, 40, ' ' ) ) {
            // std::cout << "word: " << word << std::endl;
            node_id_t val = daal::services::daal_string_to_int(&word[0], 0);
            vertices.push_back(val);
        }
    }
    file.close();
    return vertices;
}

bool vec_are_equal(const std::vector<int32_t> &v, const std::vector<int32_t> &u)
{
    std::cout << v.size() << std::endl;
    std::cout << u.size() << std::endl;
    assert(v.size() == u.size());
    for(size_t i = 0; i < v.size(); i++) {
        if(v[i] != u[i]) { return false; }
    }
    return true;
}

int main(int argc, char *argv[])
{
    CR_INIT()

    CR_ST()
    std::vector<int32_t> ref_array = read_edge_list(datasetFileName);
    CR_END("stl ref")

    CR_ST()
    std::vector<int32_t> ref_array_fpk = read_edge_list_fpk(datasetFileName);
    CR_END("fpk ref")

    CR_ST()
    std::vector<int32_t> ref_array_fpk_while = read_edge_list_fpk_while(datasetFileName);
    CR_END("fpk while")

    if(vec_are_equal(ref_array, ref_array_fpk)) {
        std::cout << "EQUAL" << std::endl;
        return 0;
    }
    std::cout << "NOT EQUAL" << std::endl;
    if(vec_are_equal(ref_array, ref_array_fpk_while)) {
        std::cout << "EQUAL" << std::endl;
        return 0;
    }


    // size_t ncols = 2;
    // size_t n_rows_ref = ref_array.size() / ncols;

    // NumericTablePtr dataTable = HomogenNumericTable<NodeId_t>::create(DictionaryIface::equal, ncols, n_rows_ref, NumericTableIface::doAllocate);
    // dataTable->basicStatistics.set(NumericTable::minimum, data_management::HomogenNumericTable<NodeId_t>::create(ncols, 1, NumericTableIface::doAllocate, nullptr));
    // dataTable->basicStatistics.set(NumericTable::maximum, data_management::HomogenNumericTable<NodeId_t>::create(ncols, 1, NumericTableIface::doAllocate, nullptr));
    // dataTable->basicStatistics.set(NumericTable::sum, data_management::HomogenNumericTable<NodeId_t>::create(ncols, 1, NumericTableIface::doAllocate, nullptr));
    // dataTable->basicStatistics.set(NumericTable::sumSquares, data_management::HomogenNumericTable<NodeId_t>::create(ncols, 1, NumericTableIface::doAllocate, nullptr));

    // FileDataSource<CSVFeatureManager, NodeId_t> dataSource(datasetFileName, DataSource::doAllocateNumericTable, DataSource::doDictionaryFromContext);
    // dataSource.getFeatureManager().setDelimiter(delimeter);
    // dataSource.loadDataBlock(dataTable.get());

    // BlockDescriptor<NodeId_t> block;
    // size_t n_rows = dataTable->getNumberOfRows();
    // dataTable->getBlockOfRows(0, n_rows, readOnly, block);
    // NodeId_t *array = block.getBlockPtr();
    // assert(ncols == block.getNumberOfColumns());

    // size_t nErr = 0;
    // for(size_t i = 0; i < ncols * n_rows_ref; i++) {
    //     if(array[i] != ref_array[i]) {nErr++; std::cout << i << ": dal read " << array[i] << "  WRONG  " << ref_array[i] << std::endl;}
    //     if(nErr == 10) { break; }
    // }
    // if(nErr == 0) {std::cout << "ALL IS CORRECT" << std::endl;}

    // dataTable->releaseBlockOfRows(block);


    return 0;
}
