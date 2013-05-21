'''
Created on Mar 25, 2013

@author: Markovtsev Vadim <v.markovtsev@samsung.com>
'''


import logging


class Feature(object):
    '''
    Stores the plan how to calculate specific speech features
    '''

    def __init__(self, name, transforms):
        self.name = name
        self.transforms = transforms

    def join(self):
        '''
        Constructs a string which describes these features calculation plan
        '''
        result = self.name + " ["
        for transform in self.transforms:
            result += transform.join() + ", "
        result = result[:-2] + "]"
        logging.debug("Constructed features description: " + result)
        return result
