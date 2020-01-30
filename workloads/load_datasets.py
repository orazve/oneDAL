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
import argparse

sys.path.append("epsilon")
sys.path.append("hepmass")
sys.path.append("higgs")
sys.path.append("mnist")
sys.path.append("susy")
sys.path.append("year_prediction_msd")

from epsilon_loader import epsilon
from hepmass_loader import hepmass
from higgs_loader import higgs
from mnist_loader import mnist
from susy_loader import susy
from year_prediction_msd_loader import year_prediction_msd

dataset_loaders = {
    "epsilon": epsilon,
    "hepmass": hepmass,
    "higgs": higgs,
    "mnist": mnist,
    "susy": susy,
    "year_prediction_msd": year_prediction_msd
}

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Use \'-d\' or \'--datasets\' option to enumerate dataset(s) which should be downloaded')
    parser.add_argument('-l', '--list', action='store_const', const=True, help='list of available datasets')
    parser.add_argument('-d', '--datasets', type=str, nargs='*', help='datasets which should be downloaded')
    args = parser.parse_args()

    if args.list:
        for key in dataset_loaders.keys():
            print(key)
        sys.exit(0)

    root_dir = os.environ['DATASETSROOT']

    if args.datasets == None:
        for val in dataset_loaders.values():
            val(root_dir)
    elif len(args.datasets) == 0:
        print('Warning: Enumerate dataset(s) which should be downloaded')
    else:
        for key,val in dataset_loaders.items():
            if key in args.datasets:
                val(root_dir)
