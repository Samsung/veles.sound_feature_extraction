'''
Created on Mar 25, 2013

@author: Markovtsev Vadim <v.markovtsev@samsung.com>
'''


import logging
from ctypes import c_char_p, c_int16, c_float, POINTER, c_void_p, c_int, \
                   byref, cast
import numpy
from spfextr.library import Library


class Extractor(object):
    '''
    Speech feature extractor.
    '''

    def __init__(self, features, buffer_size, sampling_rate):
        self.features = features
        self.buffer_size = buffer_size
        self.sampling_rate = sampling_rate
        flen = len(features)
        fstrs = (c_char_p * flen)()
        for i in range(0, flen):
            fstrs[i] = c_char_p(features[i].join().encode())
        self._config = Library().setup_features_extraction(
            fstrs, len(features), buffer_size, sampling_rate)
        if self._config:
            logging.debug("Successfully set up " + str(flen) + \
                          " features (config " + str(self._config) + ")")
        else:
            logging.error("Failed to set up features")

    def __del__(self):
        Library().destroy_features_configuration(self._config)
        logging.debug("Destroyed config " + str(self._config))

    def calculate(self, buffer):
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
            return None
        ret = {}
        for i in range(0, len(self.features)):
            length = rlengths[i] // 4
            logging.debug(self.features[i].name + " yielded " + \
                          str(length) + " results")
            res_type = c_float * length
            array_pointer = cast(results[i], POINTER(res_type))
            ret[fnames[i].decode()] = numpy.frombuffer(
                array_pointer.contents, dtype=numpy.float32,
                count=length)
        ret["RAW"] = results
        null_results = POINTER(c_void_p)()
        Library().free_results(len(self.features), fnames, null_results,
                               rlengths)
        return ret

    def free_results(self, results):
        '''
        Disposes the resources allocated for calculation results.
        '''
        if results:
            null_fnames = POINTER(c_char_p)()
            null_rlengths = POINTER(c_int)()
            Library().free_results(len(self.features), null_fnames,
                                   results["RAW"],
                                   null_rlengths)
            logging.debug("Freed extraction results " + str(results["RAW"]))
