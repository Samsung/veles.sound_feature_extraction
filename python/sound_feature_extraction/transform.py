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

    def __str__(self):
        return "Name: " + self.name + "\nDescription: " + \
            self.description + "\nDefault: " + self.default_value


class Transform(object):
    '''
    Stores information about a libSpeechFeatureExtraction transform.
    '''
    def __init__(self, name, description="",
                 supported_parameters=None, parameters=None,
                 input_format="", output_format="", condition=None):
        self.name = name
        self.description_str = description
        self.supported_parameters = supported_parameters
        self.parameters = parameters
        self.input_format = input_format
        self.output_format = output_format
        self.condition = condition

    def __str__(self):
        result = self.name + "\n" + "=" * len(self.name) + "\n\n" + \
                 self.description_str + "\n\n"
        result += "Input format: " + self.input_format + "\n"
        result += "Output format: " + self.output_format + "\n"
        if self.supported_parameters:
            result += "Supported parameters:\n"
            for pname in self.supported_parameters:
                result += "\t" + str(self.supported_parameters[pname]) \
                    .replace("\n", "\n\t") + "\n"
        if self.parameters:
            result += "Parameters:\n"
            for pname in self.parameters:
                result += "\t" + str(self.parameters[pname]) \
                    .replace("\n", "\n\t") + "\n"
        return result

    def __eq__(self, other):
        return self.name == other.name and \
            self.input_format == other.input_format and \
            self.output_format == other.output_format

    def description(self):
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