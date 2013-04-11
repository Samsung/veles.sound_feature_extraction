'''
Created on Mar 25, 2013

@author: Markovtsev Vadim <v.markovtsev@samsung.com>
'''


import logging
import unittest
from SoundFeatureExtraction.library import Library
from SoundFeatureExtraction.explorer import Explorer


class Test(unittest.TestCase):

    def setUp(self):
        logging.basicConfig(level=logging.DEBUG)

    def tearDown(self):
        pass

    def testExplorer(self):
        Library("/home/markhor/Development/SoundFeatureExtraction/build/src/"
                ".libs/libSoundFeatureExtraction.so")
        Explorer()


if __name__ == "__main__":
    # import sys;sys.argv = ['', 'Test.testExplorer']
    unittest.main()
