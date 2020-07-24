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

using namespace std;
using namespace daal;
using namespace daal::algorithms;
using namespace daal::data_management;

#define NodeId_t int32_t

/* Input data set parameters */
string datasetFileName = "../data/batch/cholesky.csv"; char delimeter = ',';
// string datasetFileName = "/nfs/inn/proj/numerics1/Users/orazvens/share/USA-road-d.USA.el"; char delimeter = ' ';

template<class node_id_t = int32_t>
std::vector<node_id_t> read_edge_list(const std::string &path)
{
    std::ifstream file(path);
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


int main(int argc, char *argv[])
{
    std::vector<int32_t> ref_array = read_edge_list(datasetFileName);

    size_t ncols = 2;
    size_t n_rows_ref = ref_array.size() / ncols;

    NumericTablePtr dataTable = HomogenNumericTable<NodeId_t>::create(DictionaryIface::equal, ncols, n_rows_ref, NumericTableIface::doAllocate);
    dataTable->basicStatistics.set(NumericTable::minimum, data_management::HomogenNumericTable<NodeId_t>::create(ncols, 1, NumericTableIface::doAllocate, nullptr));
    dataTable->basicStatistics.set(NumericTable::maximum, data_management::HomogenNumericTable<NodeId_t>::create(ncols, 1, NumericTableIface::doAllocate, nullptr));
    dataTable->basicStatistics.set(NumericTable::sum, data_management::HomogenNumericTable<NodeId_t>::create(ncols, 1, NumericTableIface::doAllocate, nullptr));
    dataTable->basicStatistics.set(NumericTable::sumSquares, data_management::HomogenNumericTable<NodeId_t>::create(ncols, 1, NumericTableIface::doAllocate, nullptr));

    FileDataSource<CSVFeatureManager, NodeId_t> dataSource(datasetFileName, DataSource::doAllocateNumericTable, DataSource::doDictionaryFromContext);
    dataSource.getFeatureManager().setDelimiter(delimeter);
    dataSource.loadDataBlock(dataTable.get());

    BlockDescriptor<NodeId_t> block;
    size_t n_rows = dataTable->getNumberOfRows();
    dataTable->getBlockOfRows(0, n_rows, readOnly, block);
    NodeId_t *array = block.getBlockPtr();
    assert(ncols == block.getNumberOfColumns());

    size_t nErr = 0;
    for(size_t i = 0; i < ncols * n_rows_ref; i++) {
        if(array[i] != ref_array[i]) {nErr++; std::cout << i << ": dal read " << array[i] << "  WRONG  " << ref_array[i] << std::endl;}
        if(nErr == 10) { break; }
    }
    if(nErr == 0) {std::cout << "ALL IS CORRECT" << std::endl;}

    dataTable->releaseBlockOfRows(block);


    return 0;
}
