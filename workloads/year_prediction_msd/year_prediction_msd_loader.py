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

def year_prediction_msd(root_dir=None):
    """
    YearPredictionMSD dataset from UCI repository (
    https://archive.ics.uci.edu/ml/datasets/yearpredictionmsd)

    TaskType:regression
    NumberOfFeatures:90
    NumberOfInstances:515345
    :param root_dir:
    """

    dataset_dir = os.path.join(root_dir, 'workloads', 'year_prediction_msd', 'dataset')

    try:
        os.makedirs(dataset_dir)
    except FileExistsError:
        pass

    year_prediction_msd_url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/00203/YearPredictionMSD.txt.zip'

    filename = os.path.join(dataset_dir, 'YearPredictionMSD.txt.zip')

    if not os.path.exists(filename):
        urlretrieve(year_prediction_msd_url, filename)

    print('year_prediction_msd dataset is downloaded')
    print('reading CSV file...')

    df = pd.read_csv(filename)

    num_train = 463715

    X_train = df.iloc[:num_train, 1:].values
    y_train = df.iloc[:num_train, 0].values
    df_train = pd.DataFrame(np.concatenate((X_train, y_train[:, None]), axis=1))
    df_train.to_csv(os.path.join(dataset_dir, 'year_prediction_msd_train.csv'), header=False, index=False)
    print('year_prediction_msd_train dataset is ready to be used')

    num_test = num_train

    X_test = df.iloc[num_test:, 1:].values
    y_test = df.iloc[num_test:, 0].values
    df_test = pd.DataFrame(np.concatenate((X_test, y_test[:, None]), axis=1))
    df_test.to_csv(os.path.join(dataset_dir, 'year_prediction_msd_test.csv'), header=False, index=False)
    print('year_prediction_msd_test dataset is ready to be used')

    X_full = df.iloc[:, 1:].values
    y_full = df.iloc[:, 0].values
    df_train = pd.DataFrame(np.concatenate((X_full, y_full[:, None]), axis=1))
    df_train.to_csv(os.path.join(dataset_dir, 'year_prediction_msd_full.csv'), header=False, index=False)
    print('year_prediction_msd_full dataset is ready to be used')

if __name__ == '__main__':
    root_dir = os.environ['DATASETSROOT']
    year_prediction_msd(root_dir)
