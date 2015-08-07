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
from ctypes import c_char_p, c_int16, c_byte, c_void_p, c_int, \
                   POINTER, byref, cast
import numpy
from libSoundFeatureExtraction.python.sound_feature_extraction.library import Library
from libSoundFeatureExtraction.python.sound_feature_extraction.formatters import Formatters
from libSoundFeatureExtraction.python.sound_feature_extraction.explorer import Explorer


class SetupFeaturesFailedException(Exception):
    """Raised when setup_features_extraction() return null.
    """
    pass


class ExtractionFailedException(Exception):
    """Raised when extract_speech_features() return status code different
    from 0.
    """
    pass


class Extractor(object):
    '''
    Speech feature extractor.
    '''

    def __init__(self, features, buffer_size, sampling_rate, channels=1):
        self.features = features
        self.features_dict = {f.name: f for f in self.features}
        self.buffer_size = buffer_size
        self.sampling_rate = sampling_rate
        self.channels = channels
        flen = len(self.features)
        fstrs = (c_char_p * flen)()
        for i in range(0, flen):
            fstrs[i] = c_char_p(self.features[i].description(
                {"sampling_rate": sampling_rate,
                 "channels": channels}).encode())
        self._config = Library().setup_features_extraction(
            fstrs, len(self.features), buffer_size, sampling_rate)
        if self._config:
            logging.debug("Successfully set up " + str(flen) + \
                          " features (config " + str(self._config) + ")")
        else:
            logging.error("Failed to set up features")
            raise SetupFeaturesFailedException()

    def __del__(self):
        Library().destroy_features_configuration(self._config)
        logging.debug("Destroyed config " + str(self._config))

    RAW_KEY_NAME = "RAW"

    def calculate_raw(self, buffer):
        '''
        Calculates the speech features.
        '''
        if not self._config:
            logging.error("Unable to calculate features")
            return None
        fnames = POINTER(c_char_p)()
        results = POINTER(c_void_p)()
        rlengths = POINTER(c_int)()
        status = Library().extract_speech_features(
            self._config, buffer.ctypes.data_as(POINTER(c_int16)),
            byref(fnames), byref(results), byref(rlengths))
        logging.debug("extract_speech_features() returned status " + \
                      str(status) + " (results = " + str(results) + ")")
        if status != 0:
            raise ExtractionFailedException()
        ret = {}
        for i in range(0, len(self.features)):
            length = rlengths[i]
            feature = self.features_dict[fnames[i].decode()]
            logging.debug(feature.name + " yielded " + \
                          str(length) + " bytes")
            res_type = c_byte * length
            array_pointer = cast(results[i], POINTER(res_type))
            format_name = feature.transforms[-1].output_format
            if format_name == "":
                format_name = Explorer().transforms[feature. \
                    transforms[-1].name].output_format
            ret[fnames[i].decode()] = Formatters.parse(numpy.frombuffer(
                array_pointer.contents, dtype=numpy.byte, count=length),
                                                       format_name)
        ret[Extractor.RAW_KEY_NAME] = results
        null_results = POINTER(c_void_p)()
        Library().free_results(len(self.features), fnames, null_results,
                               rlengths)
        logging.debug("Freed the service information")
        return ret

    def free_results(self, results):
        '''
        Disposes the resources allocated for calculation results.
        '''
        if results:
            null_fnames = POINTER(c_char_p)()
            null_rlengths = POINTER(c_int)()
            Library().free_results(len(self.features), null_fnames,
                                   results[Extractor.RAW_KEY_NAME],
                                   null_rlengths)
            logging.debug("Freed extraction results " + str(results["RAW"]))

    def calculate(self, buffer):
        '''
        Default is to copy the data from calculate_raw().
        '''
        results = self.calculate_raw(buffer)
        for name in results:
            if name != Extractor.RAW_KEY_NAME:
                results[name] = numpy.copy(results[name])
        self.free_results(results)
        del(results[Extractor.RAW_KEY_NAME])
        return results

    def report(self, file_name):
        '''
        Save the extraction report graph.
        '''
        Library().report_extraction_graph(self._config,
                                          c_char_p(file_name.encode()))
