#===============================================================================
# Copyright 2019 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#===============================================================================

import os
import sys
import pandas as pd
import numpy as np
from urllib.request import urlretrieve

def susy(root_dir=None):
    """
    SUSY dataset from UCI machine learning repository (
    https://archive.ics.uci.edu/ml/datasets/SUSY).
    - Dimensions: 5M rows, 28 columns.
    - Task: Binary classification
    :param root_dir:
    """

    dataset_dir = os.path.join(root_dir, 'workloads', 'susy', 'dataset')

    try:
        os.makedirs(dataset_dir)
    except FileExistsError:
        pass

    susy_url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/00279/SUSY.csv.gz'

    filename = os.path.join(dataset_dir, 'SUSY.csv.gz')

    if not os.path.exists(filename):
        urlretrieve(susy_url, filename)

    print('susy dataset is downloaded')
    print('reading CSV file...')

    df = pd.read_csv(filename)

    num_train = 500000

    x_train = df.iloc[:-num_train + 1, 1:].values
    y_train = df.iloc[:-num_train + 1, 0].values
    df_train = pd.DataFrame(np.concatenate((x_train, y_train[:, None]), axis=1))
    df_train.to_csv(os.path.join(dataset_dir, 'susy_train.csv'),  header=False, index=False)

    num_test = num_train

    x_test = df.iloc[-num_test:, 1:].values
    y_test = df.iloc[-num_test:, 0].values
    df_test = pd.DataFrame(np.concatenate((x_test, y_test[:, None]), axis=1))
    df_test.to_csv(os.path.join(dataset_dir, 'susy_test.csv'), header=False, index=False)

    print('susy dataset is ready to be used')

if __name__ == '__main__':
    root_dir = os.environ['DATASETSROOT']
    susy(root_dir)