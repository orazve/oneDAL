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
import re
from urllib.request import urlretrieve

def patch_epsilon(filename):
    f = open(filename, "r")
    text = f.read()
    f.close()

    patched_text = re.sub(r' [0-9]*:', ',', text)
    f = open(filename, "w")
    f.write(patched_text)
    f.close()

def epsilon(root_dir=None):
    """
    Epsilon dataset from LIBSVM Datasets (
    https://www.csie.ntu.edu.tw/~cjlin/libsvmtools/datasets/binary.html#epsilon)

    TaskType: Binary classification
    Number of features:  2000
    Number of instances: 400000
    Number of classes:   2
    :param root_dir:
    """

    dataset_dir = os.path.join(root_dir, 'workloads', 'epsilon', 'dataset')

    try:
        os.makedirs(dataset_dir)
    except FileExistsError:
        pass

    epsilon_url = 'https://www.csie.ntu.edu.tw/~cjlin/libsvmtools/datasets/binary/epsilon_normalized.bz2'

    filename = os.path.join(dataset_dir, 'epsilon_normalized.bz2')

    if not os.path.exists(filename):
        urlretrieve(epsilon_url, filename)

    print('epsilon dataset is downloaded')
    print('reading CSV file...')

    df_raw = pd.read_csv(filename)

    num_train = 30000
    csv_file = os.path.join(dataset_dir, 'epsilon_30k_train.csv')
    data_full = df_raw.iloc[:num_train, :].values
    df_train = pd.DataFrame(data_full)
    df_train.to_csv(csv_file, header=False, index=False)
    patch_epsilon(csv_file)

    df_patched = pd.read_csv(csv_file)
    X_train = df_patched.iloc[:num_train, 1:].values
    y_train = df_patched.iloc[:num_train, 0].values

    df_train = pd.DataFrame(np.concatenate((X_train, y_train[:, None]), axis=1))
    df_train.to_csv(csv_file, header=False, index=False)

    num_train = 50000
    csv_file = os.path.join(dataset_dir, 'epsilon_50k.csv')
    data_full = df_raw.iloc[:num_train, :].values
    df_train = pd.DataFrame(data_full)
    df_train.to_csv(csv_file, header=False, index=False)
    patch_epsilon(csv_file)

    df_patched = pd.read_csv(csv_file)
    X_train = df_patched.iloc[:num_train, 1:].values
    y_train = df_patched.iloc[:num_train, 0].values

    df_train = pd.DataFrame(np.concatenate((X_train, y_train[:, None]), axis=1))
    df_train.to_csv(csv_file, header=False, index=False)

    num_train = 80000
    csv_file = os.path.join(dataset_dir, 'epsilon_80k_train.csv')
    data_full = df_raw.iloc[:num_train, :].values
    df_train = pd.DataFrame(data_full)
    df_train.to_csv(csv_file, header=False, index=False)
    patch_epsilon(csv_file)

    df_patched = pd.read_csv(csv_file)
    X_train = df_patched.iloc[:num_train, 1:].values
    y_train = df_patched.iloc[:num_train, 0].values

    df_train = pd.DataFrame(np.concatenate((X_train, y_train[:, None]), axis=1))
    df_train.to_csv(csv_file, header=False, index=False)

    print('epsion dataset is ready to be used')

if __name__ == '__main__':
    root_dir = os.environ['DATASETSROOT']
    epsilon(root_dir)
