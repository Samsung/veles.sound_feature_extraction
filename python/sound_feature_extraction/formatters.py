"""
Created on May 21, 2013

@author: Markovtsev Vadim <v.markovtsev@samsung.com>
"""


import logging
import numpy


class Formatters(object):
    """
    Contains the binary deserializers for SoundFeatureExtraction formats.
    """

    @staticmethod
    def parse(array, format_name):
        if format_name.find("<") == -1:
            return array.astype(numpy.__dict__[format_name], copy=False)[0]
        if format_name.find("FixedArray<") > -1:
            length_pos = format_name.find(")") + 1
            comma_pos = format_name.find(",")
            length = int(format_name[length_pos + 1:comma_pos])
            atype = format_name[comma_pos + 2:
                                format_name.length - 1]
            res = array.astype(numpy.__dict__[atype], copy=False)
            if res.size != length:
                logging.error("Format size mismatch: real " + res.size +
                              " vs stated " + length + " (format " +
                              format_name + ")")
            return res
        if format_name.startswith("Window<"):
            atype = format_name[7:len(format_name) - 1]
            return array.astype(numpy.__dict__[atype], copy=False)
        if format_name.startswith("std::tuple"):
            logging.error("Not implemented yet")
        return numpy.copy(array)
