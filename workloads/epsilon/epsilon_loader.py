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
import re
from urllib.request import urlretrieve
import io

def get_part_epsilon(filename, num_rows):
    df = pd.read_csv(filename, nrows=num_rows, skiprows=0, header=None)

    csv_data = df.to_csv(index=False, header=False)
    patched_text = re.sub(r' [0-9]*:', ',', csv_data)
    df = pd.read_csv(io.StringIO(patched_text), header=None)

    X = df.iloc[:,1:]
    y = df[0]

    df = pd.concat([X, y], axis=1)
    return df

def epsilon(root_dir=None):
    """
    Epsilon dataset from LIBSVM Datasets (
    https://www.csie.ntu.edu.tw/~cjlin/libsvmtools/datasets/binary.html#epsilon)

    TaskType: Regression
    Number of features:  2000
    Number of instances: 400000
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

    num_train = 30000
    df_train = get_part_epsilon(filename, num_train)
    csv_file_path = os.path.join(dataset_dir, 'epsilon_30k_train.csv')
    df_train.to_csv(csv_file_path, header=False, index=False)
    del df_train
    print('epsilon_30k_train dataset is ready to be used')

    num_train = 50000
    df_train = get_part_epsilon(filename, num_train)
    csv_file_path = os.path.join(dataset_dir, 'epsilon_50k_train.csv')
    df_train.to_csv(csv_file_path, header=False, index=False)
    del df_train
    print('epsilon_50k_train dataset is ready to be used')

    num_train = 80000
    df_train = get_part_epsilon(filename, num_train)
    csv_file_path = os.path.join(dataset_dir, 'epsilon_80k_train.csv')
    df_train.to_csv(csv_file_path, header=False, index=False)
    del df_train
    print('epsilon_80k_train dataset is ready to be used')

if __name__ == '__main__':
    root_dir = os.environ['DATASETSROOT']
    epsilon(root_dir)
