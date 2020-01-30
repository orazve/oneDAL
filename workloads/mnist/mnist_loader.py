#===============================================================================
# Copyright 2019-2020 Intel Corporation
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

def mnist(root_dir=None):
    """
    Mnist dataset from Joseph Chet Redmon site (
    https://pjreddie.com).
    - Dimensions (Train): 60k rows, 784 columns.
    - Dimensions (Test): 10k rows, 784 columns.
    - Task: Classification
    :param root_dir:
    """

    dataset_dir = os.path.join(root_dir, 'workloads', 'mnist', 'dataset')

    try:
        os.makedirs(dataset_dir)
    except FileExistsError:
        pass

    mnist_train_url = 'https://pjreddie.com/media/files/mnist_train.csv'
    mnist_test_url = 'https://pjreddie.com/media/files/mnist_test.csv'

    filename_mnist_train = os.path.join(dataset_dir, 'mnist_train.csv')
    filename_mnist_test = os.path.join(dataset_dir, 'mnist_test.csv')

    if not os.path.exists(filename_mnist_train):
        urlretrieve(mnist_train_url, filename_mnist_train)
    if not os.path.exists(filename_mnist_test):
        urlretrieve(mnist_test_url, filename_mnist_test)

    print('mnist dataset is downloaded')
    print('reading CSV file...')

    df_mnist_train = pd.read_csv(filename_mnist_train)
    df_mnist_test = pd.read_csv(filename_mnist_test)

    X_train = df_mnist_train.iloc[:, 1:].values
    y_train = df_mnist_train.iloc[:, 0].values
    df_train = pd.DataFrame(np.concatenate((X_train, y_train[:, None]), axis=1))
    df_train.to_csv(os.path.join(dataset_dir, 'mnist_train.csv'), header=False, index=False)
    print('mnist_train dataset is ready to be used')

    X_test = df_mnist_test.iloc[:, 1:].values
    y_test = df_mnist_test.iloc[:, 0].values
    df_test = pd.DataFrame(np.concatenate((X_test, y_test[:, None]), axis=1))
    df_test.to_csv(os.path.join(dataset_dir, 'mnist_test.csv'), header=False, index=False)
    print('mnist_test dataset is ready to be used')

if __name__ == '__main__':
    root_dir = os.environ['DATASETSROOT']
    mnist(root_dir)
