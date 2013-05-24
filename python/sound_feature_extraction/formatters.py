"""
Created on May 21, 2013

@author: Markovtsev Vadim <v.markovtsev@samsung.com>
"""


import ctypes
import logging
import numpy


class Formatters(object):
    """
    Contains the binary deserializers for SoundFeatureExtraction formats.
    """

    @staticmethod
    def reinterpret_cast(array, type_name):
        return array.view(ctypes.__dict__["c_" + type_name])

    @staticmethod
    def parse(array, format_name):
        if format_name.find("<") == -1:
            conv_array = Formatters.reinterpret_cast(array, format_name)
            if conv_array.size > 1:
                return conv_array
            else:
                return conv_array[0]

        if format_name.find("FixedArray<") > -1:
            length_pos = format_name.find(")") + 1
            comma_pos = format_name.find(",")
            length = int(format_name[length_pos + 1:comma_pos])
            atype = format_name[comma_pos + 2:
                                format_name.length - 1]
            res = Formatters.reinterpret_cast(array, atype)
            if res.size != length:
                logging.error("Format size mismatch: real " + res.size +
                              " vs stated " + length + " (format " +
                              format_name + ")")
            return res
        if format_name.startswith("Window<"):
            atype = format_name[7:len(format_name) - 1]
            return Formatters.reinterpret_cast(array, atype)
        if format_name.startswith("std::tuple"):
            logging.error("Not implemented yet")
        return numpy.copy(array)
