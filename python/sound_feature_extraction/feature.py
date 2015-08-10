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
import re
from .transform import Transform
from .explorer import Explorer


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

    def description(self, cond_locals={}):
        '''
        Constructs a string which describes this feature calculation plan
        '''
        result = self.name + " ["
        for transform in self.transforms:
            add = True
            if transform.condition:
                add = eval(transform.condition, cond_locals)
            if add:
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
        for tm in re.finditer("\\w+\\s*(\\([^\\)]*\\))?\\s*(\\{[^\\}]+\\})?",
                              tdescription):
            trname = re.match("\\w+", tm.group()).group()
            parameters = {}
            pstrm = re.search("(?<=\\()[^\\)]*(?=\\))", tm.group())
            if pstrm:
                for line in pstrm.group().split(","):
                    kv = line.split("=")
                    parameters[kv[0].strip()] = kv[1].strip()
            trcond = re.search("\\{[^\\}]+\\}$", tm.group())
            cond = trcond.group()[1:-1] if trcond else None
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
                                            regtr.output_format,
                                            cond))
            else:
                transforms.append(Transform(trname,
                                            parameters=parameters,
                                            condition=cond))
        if transforms[-1].output_format == "":
            raise FeatureFormatNotRecognizedException(name)
        return Feature(name, transforms)
