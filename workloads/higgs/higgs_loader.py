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

def higgs(root_dir=None):
    """
    Higgs dataset from UCI machine learning repository (
    https://archive.ics.uci.edu/ml/datasets/HIGGS).
    - Dimensions: 11M rows, 28 columns.
    - Task: Binary classification
    :param root_dir:
    """

    dataset_dir = os.path.join(root_dir, 'workloads', 'higgs', 'dataset')

    try:
        os.makedirs(dataset_dir)
    except FileExistsError:
        pass

    higgs_url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/00280/HIGGS.csv.gz'

    filename = os.path.join(dataset_dir, 'HIGGS.csv.gz')

    if not os.path.exists(filename):
        urlretrieve(higgs_url, filename)

    print('higgs dataset is downloaded')
    print('reading CSV file...')

    df = pd.read_csv(filename)

    num_train = 2000000
    X_train = df.iloc[:num_train, 1:].values
    y_train = df.iloc[:num_train, 0].values
    df_train = pd.DataFrame(np.concatenate((X_train, y_train[:, None]), axis=1))
    df_train.to_csv(os.path.join(dataset_dir, 'higgs_2m_train.csv'), header=False, index=False)
    print('higgs_2m_train dataset is ready to be used')

    num_train = 1000000
    X_train = df.iloc[:num_train, 1:].values
    y_train = df.iloc[:num_train, 0].values
    df_train = pd.DataFrame(np.concatenate((X_train, y_train[:, None]), axis=1))
    df_train.to_csv(os.path.join(dataset_dir, 'higgs_1m_train.csv'), header=False, index=False)
    print('higgs_1m_train dataset is ready to be used')

    num_test = 1500000
    X_test = df.iloc[num_train:num_test, 1:].values
    y_test = df.iloc[num_train:num_test, 0].values
    df_test = pd.DataFrame(np.concatenate((X_test, y_test[:, None]), axis=1))
    df_test.to_csv(os.path.join(dataset_dir, 'higgs_500t_test.csv'), header=False, index=False)
    print('higgs_500t_test dataset is ready to be used')

    num_train = 100000
    X_train = df.iloc[:num_train, 1:].values
    y_train = df.iloc[:num_train, 0].values
    df_train = pd.DataFrame(np.concatenate((X_train, y_train[:, None]), axis=1))
    df_train.to_csv(os.path.join(dataset_dir,'higgs_100t_train.csv'), header=False, index=False)
    print('higgs_100t_train dataset is ready to be used')

    num_test = 150000
    X_test = df.iloc[num_train:num_test, 1:].values
    y_test = df.iloc[num_train:num_test, 0].values
    df_test = pd.DataFrame(np.concatenate((X_test, y_test[:, None]), axis=1))
    df_test.to_csv(os.path.join(dataset_dir, 'higgs_50t_test.csv'), header=False, index=False)
    print('higgs_50t_test dataset is ready to be used')

if __name__ == '__main__':
    root_dir = os.environ['DATASETSROOT']
    higgs(root_dir)
