'''
Created on Mar 25, 2013

@author: Markovtsev Vadim <v.markovtsev@samsung.com>
'''


import logging
import numpy
import unittest
from spfextr.extractor import Extractor
from spfextr.features import Features
from spfextr.library import Library
from spfextr.transform import Transform


class Test(unittest.TestCase):

    def setUp(self):
        logging.basicConfig(level=logging.DEBUG)
        Library("/home/markhor/Development/spfextr/build/src/"
                ".libs/libspfextr.so")

    def tearDown(self):
        pass

    def testExtractor(self):
        extr = Extractor([Features("MFCC",
                                   [Transform("Window",
                                              parameters={"length": "32"}),
                                    Transform("RDFT"),
                                    Transform("Energy"),
                                    Transform("FilterBank"),
                                    Transform("FilterBank"),
                                    Transform("Log"),
                                    Transform("Selector",
                                              parameters={"length": "256"}),
                                    Transform("DCT"),
                                    Transform("Selector",
                                              parameters={"length": "24"})]
                                   )],
                         48000, 16000)
        buffer = numpy.ones((48000,), dtype=numpy.int16)
        results = extr.calculate(buffer)
        print("Calculated results:")
        print(results["MFCC"])
        extr.free_results(results)

if __name__ == "__main__":
    # import sys;sys.argv = ['', 'Test.testExtractor']
    unittest.main()
