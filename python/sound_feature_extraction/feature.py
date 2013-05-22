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


class TransformNotRegisteredException(Exception):
    """The requested transform is not registered in libSoundFeatureExtraction.
    """
    def __init__(self, name):
        super(Exception, self).__init__()
        self.name = name

    def __str__(self):
        return "Transform \"" + self.name + "\" is not registered"


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
                if not regtr:
                    raise TransformNotRegisteredException(trname)
            transforms.append(Transform(trname, regtr.description,
                                        regtr.supported_parameters,
                                        parameters,
                                        regtr.input_format,
                                        regtr.output_format))
        return Feature(name, transforms)
