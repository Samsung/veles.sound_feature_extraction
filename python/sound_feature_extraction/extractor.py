"""
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
"""


import logging
import numpy
from .library import Library
from .formatters import Formatters
from .explorer import Explorer


class SetupFeaturesFailedException(Exception):
    """Raised when setup_features_extraction() return null.
    """
    pass


class ExtractionFailedException(Exception):
    """Raised when extract_sound_features() return status code different
    from 0.
    """
    pass


class Extractor(object):
    """
    Audio feature extractor.
    """

    logger = logging.getLogger("sfm.Extractor")

    def __init__(self, features, buffer_size, sampling_rate, channels=1):
        self._config = None
        self.features = features
        self.features_dict = {f.name: f for f in self.features}
        self.buffer_size = buffer_size
        self.sampling_rate = sampling_rate
        self.channels = channels
        flen = len(self.features)
        fstrs = Library().new("char*[]", flen)
        # prevent from garbage collecting fstrs contents
        fstrs_ref = [None] * flen
        for i, f in enumerate(self.features):
            fstrs[i] = fstrs_ref[i] = Library().new("char[]", f.description(
                {"sampling_rate": sampling_rate,
                 "channels": channels}).encode())
        self._config = Library().setup_features_extraction(
            fstrs, len(self.features), buffer_size, sampling_rate)
        # fstrs_ref is still alive at this point
        del fstrs_ref
        if self._config:
            self.logger.debug("Successfully set up %d features (config %s)",
                              flen, self._config)
        else:
            self.logger.error("Failed to set up features")
            raise SetupFeaturesFailedException()

    def __del__(self):
        if not self._config:
            return
        Library().destroy_features_configuration(self._config)
        self.logger.debug("Destroyed config %s", self._config)

    RAW_KEY_NAME = "RAW"

    def calculate_raw(self, buffer):
        """
        Calculates the audio features.
        """
        if not self._config:
            self.logger.error("Unable to calculate features")
            return None
        fnames = Library().new("char***")
        results = Library().new("void***")
        rlengths = Library().new("int**")
        status = Library().extract_sound_features(
            self._config, Library().cast(
                "int16_t*", buffer.__array_interface__["data"][0]),
            fnames, results, rlengths)
        self.logger.debug("extract_speech_features() returned status %d "
                          "(results = %s)", status, results[0])
        if status != 0:
            raise ExtractionFailedException()
        ret = {}
        for i in range(len(self.features)):
            length = rlengths[0][i]
            fname = Library().string(fnames[0][i]).decode()
            feature = self.features_dict[fname]
            self.logger.debug(feature.name + " yielded %d bytes", length)
            buffer = Library().buffer(results[0][i], length)
            format_name = feature.transforms[-1].output_format
            if format_name == "":
                format_name = Explorer().transforms[
                    feature.transforms[-1].name].output_format
            ret[fname] = Formatters.parse(numpy.frombuffer(
                buffer, dtype=numpy.byte, count=length), format_name)
        ret[Extractor.RAW_KEY_NAME] = results[0]
        Library().free_results(len(self.features), fnames[0],
                               Library().NULL, rlengths[0])
        self.logger.debug("Freed the service information")
        return ret

    def free_results(self, results):
        """
        Disposes the resources allocated for calculation results.
        """
        if results:
            Library().free_results(len(self.features), Library().NULL,
                                   results[Extractor.RAW_KEY_NAME],
                                   Library().NULL)
            self.logger.debug("Freed extraction results %s", results["RAW"])

    def calculate(self, buffer):
        """
        Default is to copy the data from calculate_raw().
        """
        results = self.calculate_raw(buffer)
        for name in results:
            if name != Extractor.RAW_KEY_NAME:
                results[name] = numpy.copy(results[name])
        self.free_results(results)
        del(results[Extractor.RAW_KEY_NAME])
        return results

    def report(self, file_name):
        """
        Saves the extraction report graph.
        """
        Library().report_extraction_graph(
            self._config, Library().new("char[]", file_name))
