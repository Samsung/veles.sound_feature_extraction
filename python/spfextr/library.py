'''
Created on Mar 21, 2013

@author: Markovtsev Vadim <v.markovtsev@samsung.com>
'''


import logging
from ctypes import cdll, POINTER, c_char_p, c_int, c_int16, c_size_t, \
                   c_float, c_void_p, Structure


FeatureExtractionResult = c_int


class FeaturesConfiguration(Structure):
    pass


class Library(object):
    '''
    Loads the spfextr.so shared library and wraps the handle.
    '''

    def __init__(self, path=None):
        if not self._handle:
            if not path:
                logging.warning("Library path was not specified, "
                                 "will use the default")
                path = "libspfextr.so"
            try:
                logging.debug("Trying to load " + path + "...")
                self._handle = cdll.LoadLibrary(path)
            except OSError:
                logging.critical("Failed to load " + path)
                return
            logging.debug("Success. Loading functions...")
            self.MAX_FEATURES_COUNT = 256
            self.FEATURE_EXTRACTION_RESULT_OK = 0
            self.FEATURE_EXTRACTION_RESULT_ERROR = \
                (self.FEATURE_EXTRACTION_RESULT_OK + 1)
            self._handle.query_transforms_list.argtypes = \
                [POINTER(POINTER(c_char_p)), POINTER(c_int)]
            self._handle.query_transforms_list.restype = None
            self._handle.destroy_transforms_list.argtypes = \
                [POINTER(c_char_p), c_int]
            self._handle.destroy_transforms_list.restype = None
            self._handle.query_transform_details.argtypes = \
                [c_char_p, POINTER(c_char_p), POINTER(c_char_p),
                 POINTER(c_char_p), POINTER(POINTER(c_char_p)),
                 POINTER(POINTER(c_char_p)), POINTER(POINTER(c_char_p)),
                 POINTER(c_int)]
            self._handle.query_transform_details.restype = None
            self._handle.destroy_transform_details.argtypes = \
                [c_char_p, c_char_p, c_char_p, POINTER(c_char_p),
                 POINTER(c_char_p), POINTER(c_char_p), c_int]
            self._handle.destroy_transform_details.restype = None
            self._handle.setup_features_extraction.argtypes = \
                [POINTER(c_char_p), c_int, c_size_t, c_int]
            self._handle.setup_features_extraction.restype = \
                POINTER(FeaturesConfiguration)
            self._handle.extract_speech_features.argtypes = \
                [POINTER(FeaturesConfiguration), POINTER(c_int16),
                 POINTER(POINTER(c_char_p)), POINTER(POINTER(c_void_p)),
                 POINTER(POINTER(c_int))]
            self._handle.extract_speech_features.restype = \
                FeatureExtractionResult
            self._handle.report_extraction_time.argtypes = \
                [POINTER(FeaturesConfiguration), POINTER(POINTER(c_char_p)),
                 POINTER(POINTER(c_float)), POINTER(c_int)]
            self._handle.report_extraction_time.restype = None
            self._handle.destroy_extraction_time_report.argtypes = \
                [POINTER(c_char_p), POINTER(c_float), c_int]
            self._handle.destroy_extraction_time_report.restype = None
            self._handle.report_extraction_graph.argtypes = \
                [POINTER(FeaturesConfiguration), c_char_p]
            self._handle.report_extraction_graph.restype = None
            self._handle.destroy_features_configuration.argtypes = \
                [POINTER(FeaturesConfiguration)]
            self._handle.destroy_features_configuration.restype = None
            self._handle.free_results.argtypes = \
                [c_int, POINTER(c_char_p), POINTER(c_void_p), POINTER(c_int)]
            self._handle.free_results.restype = None
            logging.debug("Finished loading functions")

    def __new__(cls, path=None):
        if not cls._instance:
            cls._instance = super(Library, cls).__new__(cls, path)
        return cls._instance

    def __str__(self):
        return 'libSpeechFeatureExtraction cdll library handle ' + self._handle

    def __getattr__(self, attr):
        if not self._handle:
            logging.error("Attempted to invoke a function but the library "
                          "was not loaded")
            return None
        return self._handle.__getattribute__(attr)

    _instance = None
    _handle = None
