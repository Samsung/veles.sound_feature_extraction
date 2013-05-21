'''
Created on Mar 25, 2013

@author: Markovtsev Vadim <v.markovtsev@samsung.com>
'''


import logging


class TransformParameter(object):
    '''
    Stores information about a transform's parameter
    '''
    def __init__(self, name, description, default_value):
        self.name = str(name)
        self.description = str(description)
        self.default_value = str(default_value)


class Transform(object):
    '''
    Stores information about a libSpeechFeatureExtraction transform.
    '''
    def __init__(self, name, description="",
                 supported_parameters=None, parameters=None,
                 input_format="", output_format=""):
        self.name = name
        self.description = description
        self.supported_parameters = supported_parameters
        self.parameters = parameters
        self.input_format = input_format
        self.output_format = output_format

    def __str__(self):
        result = self.name + "\n" + "=" * len(self.name) + "\n\n" + \
                 self.description + "\n\n"
        result += "Input format: " + self.input_format + "\n"
        result += "Output format: " + self.output_format + "\n"
        if self.parameters:
            result += "Parameters:\n"
            for param in self.parameters:
                result += param + "\n"
        return result

    def __eq__(self, other):
        return self.name == other.name and \
            self.input_format == other.input_format and \
            self.output_format == other.output_format

    def join(self):
        '''
        Constructs a string which describes this transform and it's parameters.
        '''
        res = self.name
        if self.parameters:
            res += "("
            for key, value in self.parameters.items():
                res += key + " = " + value + ", "
            res = res[:-2] + ")"
        logging.debug("Constructed transform string: " + res)
        return res
