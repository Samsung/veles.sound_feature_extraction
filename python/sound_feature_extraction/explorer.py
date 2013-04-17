'''
Created on Mar 21, 2013

@author: Markovtsev Vadim <v.markovtsev@samsung.com>
'''


import logging
from ctypes import POINTER, c_char_p, c_int, byref
from sound_feature_extraction.library import Library
from sound_feature_extraction.transform import Transform, TransformParameter


class Explorer(object):
    '''
    Provides information about implemented transforms.
    '''

    def __init__(self):
        if not self.transforms:
            names = POINTER(c_char_p)()
            list_size = c_int()
            logging.debug("query_transforms_list()")
            Library().query_transforms_list(byref(names), byref(list_size))
            logging.debug("Got the list of " + str(list_size.value) + \
                          " transforms")
            self.transforms = {}
            for i in range(0, list_size.value):
                description = c_char_p()
                input_format = c_char_p()
                output_format = c_char_p()
                p_names = POINTER(c_char_p)()
                p_descs = POINTER(c_char_p)()
                p_defs = POINTER(c_char_p)()
                p_count = c_int()
                transform_name = names[i].decode()
                logging.debug("query_transform_details(" + \
                              transform_name + ")")
                Library().query_transform_details(names[i], byref(description),
                                                  byref(input_format),
                                                  byref(output_format),
                                                  byref(p_names),
                                                  byref(p_descs),
                                                  byref(p_defs),
                                                  byref(p_count))
                parameters = {}
                for j in range(0, p_count.value):
                    p_name = p_names[j].decode()
                    parameters[p_name] = TransformParameter(
                        p_names[j].decode(), p_descs[j].decode(),
                        p_defs[j].decode())
                self.transforms[transform_name] = Transform(transform_name,
                                                           description.value,
                                                           parameters, None,
                                                           input_format.value,
                                                           output_format.value)
                Library().destroy_transform_details(description, input_format,
                                                    output_format, p_names,
                                                    p_descs, p_defs, p_count)
            Library().destroy_transforms_list(names, list_size)
            logging.debug("Done with getting transforms")

    def __new__(cls):
        if not cls._instance:
            cls._instance = super(Explorer, cls).__new__(cls)
        return cls._instance

    _instance = None
    transforms = None
