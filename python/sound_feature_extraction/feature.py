'''
Created on Mar 25, 2013

@author: Markovtsev Vadim <v.markovtsev@samsung.com>
'''


import logging
import re
from sound_feature_extraction.transform import Transform
from sound_feature_extraction.explorer import Explorer


class ParsingFeatureFailedException(Exception):
    """Raised when we fail to parse the feature description.
    """
    pass


class FeatureFormatNotRecognizedException(Exception):
    """Could not recognize the output format of the feature.
    """
    def __init__(self, name):
        super(Exception, self).__init__()
        self.name = name

    def __str__(self):
        return "Feature \"" + self.name + "\"'s output format " + \
               "is not recognized"


class Feature(object):
    '''
    Stores the plan how to calculate a specific feature
    '''

    def __init__(self, name, transforms):
        self.name = name
        self.transforms = transforms

    def description(self):
        '''
        Constructs a string which describes this feature calculation plan
        '''
        result = self.name + " ["
        for transform in self.transforms:
            result += transform.description() + ", "
        result = result[:-2] + "]"
        logging.debug("Constructed feature description: " + result)
        return result

    def __str__(self):
        res = self.name + ":\n"
        i = 1
        for transform in self.transforms:
            res += "\t" + str(i) + ". " + transform.name
            if transform.parameters and len(transform.parameters) > 0:
                res += " ("
                for param in transform.parameters:
                    res += param + " = " + transform.parameters[param] + ","
                res = res[:-1]
                res += ")"
            res += "\n"
        return res

    @staticmethod
    def from_string(description):
        fnm = re.match("\\s*\\w+(?=\\s*\\[([^\\]]+)\\]\\s*)", description)
        if not fnm:
            raise ParsingFeatureFailedException()
        name = fnm.group().strip()
        transforms = []
        tdescription = re.search("\\[([^\\]]+)\\]", description).group()
        for tm in re.finditer("\\w+\\s*(\\([^\\)]*\\))?", tdescription):
            trname = re.match("\\w+", tm.group()).group()
            parameters = {}
            pstrm = re.search("(?<=\\()[^\\)]*(?=\\))", tm.group())
            if pstrm:
                for line in pstrm.group().split(","):
                    kv = line.split("=")
                    parameters[kv[0].strip()] = kv[1].strip()
            regtr = Explorer().transforms.get(trname)
            if not regtr:
                if len(transforms) > 0:
                    regtr = Explorer().transforms.get(
                        trname + " (" + transforms[-1].output_format + ")")
                else:
                    regtr = Explorer().transforms.get(trname + " (Raw<short>)")
            if regtr:
                transforms.append(Transform(trname, regtr.description,
                                            regtr.supported_parameters,
                                            parameters,
                                            regtr.input_format,
                                            regtr.output_format))
            else:
                transforms.append(Transform(trname,
                                            parameters=parameters))
        if transforms[-1].output_format == "":
            raise FeatureFormatNotRecognizedException(name)
        return Feature(name, transforms)
