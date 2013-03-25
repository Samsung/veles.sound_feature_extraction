'''
Created on Mar 25, 2013

@author: Markovtsev Vadim <v.markovtsev@samsung.com>
'''


import logging
import unittest
from spfextr.library import Library
from spfextr.explorer import Explorer


class Test(unittest.TestCase):

    def setUp(self):
        logging.basicConfig(level=logging.DEBUG)

    def tearDown(self):
        pass

    def testExplorer(self):
        Library("/home/markhor/Development/spfextr/build/src/"
                ".libs/libspfextr.so")
        Explorer()


if __name__ == "__main__":
    # import sys;sys.argv = ['', 'Test.testExplorer']
    unittest.main()
