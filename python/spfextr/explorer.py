'''
Created on Mar 21, 2013

@author: Markovtsev Vadim <v.markovtsev@samsung.com>
'''


import logging
from library import Library
from ctypes import POINTER, c_char_p, c_int, byref


class TransformParameter(object):
    def __init__(self, name, description, default_value):
        self.name = str(name)
        self.description = str(description)
        self.default_value = str(default_value)


class Transform(object):
    """
    Stores information about a libSpeechFeatureExtraction transform.
    """
    def __init__(self, name, description, parameters,
                 inputFormat, outputFormat):
        self.name = name
        self.description = description
        self.parameters = parameters
        self.input_format = inputFormat
        self.output_format = outputFormat

    def __str__(self):
        result = self.name + "\n" + "=" * len(self.name) + "\n\n" + \
                 self.description + "\n\nParameters:\n"
        for param in self.parameters:
            result += param + "\n"
        return result


class Explorer(object):
    """
    Provides information about implemented transforms.
    """

    def __init__(self):
        if not self.transforms:
            names = POINTER(c_char_p)()
            listSize = c_int()
            logging.debug("query_transforms_list()")
            Library().query_transforms_list(byref(names), byref(listSize))
            logging.debug("Got the list of " + str(listSize.value) + \
                          " transforms")
            self.transforms = {}
            for i in range(0, listSize.value):
                description = c_char_p()
                input_format = c_char_p()
                output_format = c_char_p()
                pNames = POINTER(c_char_p)()
                pDescs = POINTER(c_char_p)()
                pDefs = POINTER(c_char_p)()
                pCount = c_int()
                transformName = names[i].decode()
                logging.debug("query_transform_details(" + \
                              transformName + ")")
                Library().query_transform_details(names[i], byref(description),
                                                  byref(input_format),
                                                  byref(output_format),
                                                  byref(pNames), byref(pDescs),
                                                  byref(pDefs), byref(pCount))
                parameters = {}
                for j in range(0, pCount.value):
                    pName = pNames[j].decode()
                    parameters[pName] = TransformParameter(pNames[j].decode(),
                                                           pDescs[j].decode(),
                                                           pDefs[j].decode())
                self.transforms[transformName] = Transform(transformName,
                                                           description.value,
                                                           input_format.value,
                                                           output_format.value,
                                                           parameters)
                Library().destroy_transform_details(description, input_format,
                                                    output_format, pNames,
                                                    pDescs, pDefs, pCount)
            Library().destroy_transforms_list(names, listSize)
            logging.debug("Done with getting transforms")

    def __new__(cls):
        if not cls._instance:
            cls._instance = super(Explorer, cls).__new__(cls)
        return cls._instance

    _instance = None
    transforms = None


def main():
    logging.basicConfig(level=logging.DEBUG)
    Library("/home/markhor/Development/spfextr/build/src/.libs/libspfextr.so")
    Explorer()

if __name__ == '__main__':
    main()
