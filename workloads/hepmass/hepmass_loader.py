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

def hepmass(root_dir=None):
    """
    HEPMASS dataset from UCI machine learning repository (
    https://archive.ics.uci.edu/ml/datasets/HEPMASS).
    - Dimensions: 10.5M rows, 28 columns.
    - Task: Binary classification; clustering
    :param root_dir:
    """

    dataset_dir = os.path.join(root_dir, 'workloads', 'hepmass', 'dataset')

    try:
        os.makedirs(dataset_dir)
    except FileExistsError:
        pass

    test_url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/00347/all_test.csv.gz'
    train_url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/00347/all_train.csv.gz'

    filename_test = os.path.join(dataset_dir, 'all_test.csv.gz')
    filename_train = os.path.join(dataset_dir, 'all_train.csv.gz')

    if not os.path.exists(filename_test):
        urlretrieve(test_url, filename_test)
    if not os.path.exists(filename_train):
        urlretrieve(train_url, filename_train)

    print('hepmass train dataset is downloaded')
    print('reading CSV file...')

    df_train_raw = pd.read_csv(filename_train)

    num_train = 1000000

    X_train = df_train_raw.iloc[:num_train, 1:].values
    y_train = df_train_raw.iloc[:num_train, 0].values
    df_train = pd.DataFrame(np.concatenate((X_train, y_train[:, None]), axis=1))
    df_train.to_csv(os.path.join(dataset_dir, 'hepmass_1m_train.csv'), header=False, index=False)

    print('hepmass test dataset is downloaded')
    print('reading CSV file...')

    df_test_raw = pd.read_csv(filename_test)

    num_test = 500000

    X_test = df_test_raw.iloc[:num_test, 1:].values
    y_test = df_test_raw.iloc[:num_test, 0].values
    df_test = pd.DataFrame(np.concatenate((X_test, y_test[:, None]), axis=1))
    df_test.to_csv(os.path.join(dataset_dir, 'hepmass_500t_test.csv'), header=False, index=False)

    print('hepmass dataset is ready to be used')

if __name__ == '__main__':
    root_dir = os.environ['DATASETSROOT']
    hepmass(root_dir)