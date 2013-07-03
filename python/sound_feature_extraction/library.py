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


class LoadingLibraryFailedException(Exception):
    """Raised when we fail to load the SoundFeatureExtraction native library.
    """
    pass


class Library(object):
    '''
    Loads the SoundFeatureExtraction.so shared library and wraps the handle.
    '''

    def __init__(self, path=None):
        if not self._handle or (path != self._path and path and path != ""):
            if not path or path == "":
                logging.info("Library path was not specified, "
                             "will use the default")
                path = "libSoundFeatureExtraction.so"
            self._path = path
            try:
                logging.debug("Trying to load " + path + "...")
                self._handle = cdll.LoadLibrary(path)
            except OSError:
                logging.critical("Failed to load " + path)
                raise LoadingLibraryFailedException()
            logging.debug("Success. Loading functions...")
            try:
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
                self._handle.query_format_converters_list.argtypes = \
                    [POINTER(POINTER(c_char_p)), POINTER(POINTER(c_char_p)),
                     POINTER(c_int)]
                self._handle.query_format_converters_list.restype = None
                self._handle.destroy_format_converters_list.argtypes = \
                    [POINTER(c_char_p), POINTER(c_char_p), c_int]
                self._handle.destroy_format_converters_list.restype = None
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
                    [POINTER(FeaturesConfiguration),
                     POINTER(POINTER(c_char_p)),
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
                    [c_int, POINTER(c_char_p), POINTER(c_void_p),
                     POINTER(c_int)]
                self._handle.free_results.restype = None
                self._handle.get_omp_transforms_max_threads_num.argtypes = \
                    None
                self._handle.get_omp_transforms_max_threads_num.restype = \
                    c_int
                self._handle.set_omp_transforms_max_threads_num.argtypes = \
                    [c_int]
                self._handle.set_omp_transforms_max_threads_num.restype = \
                    None
                self._handle.get_use_simd.argtypes = None
                self._handle.get_use_simd.restype = c_int
                self._handle.set_use_simd.argtypes = [c_int]
                self._handle.set_use_simd.restype = None
            except:
                raise LoadingLibraryFailedException()
            logging.debug("Finished loading functions")

    def __new__(cls, path=None):
        if not cls._instance:
            logging.debug("Initializing a new instance of Library class "
                          "(path is " + str(path) + ")")
            cls._instance = super(Library, cls).__new__(cls)
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
    _path = None
