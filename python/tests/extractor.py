'''
Created on Mar 25, 2013

@author: Markovtsev Vadim <v.markovtsev@samsung.com>

███████████████████████████████████████████████████████████████████████████████

Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.

███████████████████████████████████████████████████████████████████████████████
'''


import logging
import numpy
import unittest
from sound_feature_extraction.extractor import Extractor
from sound_feature_extraction.feature import Feature
from sound_feature_extraction.library import Library
from sound_feature_extraction.transform import Transform


class Test(unittest.TestCase):

    def setUp(self):
        logging.basicConfig(level=logging.DEBUG)
        Library("/home/markhor/Development/SoundFeatureExtraction/build/src/"
                ".libs/libSoundFeatureExtraction.so")

    def tearDown(self):
        pass

    def testExtractor(self):
        extr = Extractor([Feature("MFCC",
                                  [Transform("Window",
                                             parameters={"length": "512"}),
                                   Transform("RDFT"),
                                   Transform("SpectralEnergy"),
                                   Transform("FilterBank",
					     parameters={"squared": "true"}),
                                   Transform("Log"),
                                   Transform("DCT"),
                                   Transform("Selector",
                                             parameters={"length": "16"})]
                                  )],
                         48000, 16000)
        buffer = numpy.ones(48000, dtype=numpy.int16)
        buffer *= 1000
        results = extr.calculate(buffer)
        print("Calculated results:")
        print(results["MFCC"])

if __name__ == "__main__":
    # import sys;sys.argv = ['', 'Test.testExtractor']
    unittest.main()
