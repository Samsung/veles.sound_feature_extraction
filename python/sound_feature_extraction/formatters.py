"""
Created on May 21, 2013

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
"""

import ctypes
import logging
import numpy


class Formatters(object):
    """
    Contains the binary deserializers for SoundFeatureExtraction formats.
    """

    logger = logging.getLogger("sfm.Formatters")

    @staticmethod
    def reinterpret_cast(array, type_name):
        return array.view(ctypes.__dict__["c_" + type_name])

    @staticmethod
    def parse(array, format_name):
        if format_name.find("<") == -1:
            if format_name.endswith("*"):
                format_name = format_name[:-1]
            conv_array = Formatters.reinterpret_cast(array, format_name)
            if conv_array.size > 1:
                return conv_array
            else:
                return conv_array[0]

        if format_name.find("FixedArray<") > -1:
            length_pos = format_name.find(")")
            comma_pos = format_name.find(",")
            length = int(format_name[length_pos + 1:comma_pos])
            atype = format_name[comma_pos + 2:
                                len(format_name) - 2]
            res = Formatters.reinterpret_cast(array, atype)
            if res.size % length != 0:
                Formatters.logger.error(
                    "Format size mismatch: real %d is not a multiple of %d"
                    " (format %s)", res.size, length, format_name)
            return res
        if format_name.startswith("Window<"):
            atype = format_name[7:len(format_name) - 1]
            return Formatters.reinterpret_cast(array, atype)
        if format_name.startswith("std::tuple"):
            Formatters.logger.error("Not implemented yet")
        return numpy.copy(array)
