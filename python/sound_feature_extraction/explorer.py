'''
Created on Mar 21, 2013

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


import collections
import logging
from .library import Library
from .transform import Transform, TransformParameter


class Explorer(object):
    '''
    Provides information about implemented transforms.
    '''
    logger = logging.getLogger("sfm.Explorer")

    def __init__(self):
        if not self.transforms:
            names = Library().new("char***")
            list_size = Library().new("int*")
            self.logger.debug("query_transforms_list()")
            Library().query_transforms_list(names, list_size)
            self.logger.debug("Got the list of %d transforms", list_size[0])
            self.transforms = {}
            for i in range(list_size[0]):
                description = Library().new("char**")
                input_format = Library().new("char**")
                output_format = Library().new("char**")
                p_names = Library().new("char***")
                p_descs = Library().new("char***")
                p_defs = Library().new("char***")
                p_count = Library().new("int*")
                transform_name = Library().string(names[0][i]).decode()
                self.logger.debug("query_transform_details(%s)",
                                  transform_name)
                Library().query_transform_details(
                    names[0][i], description, input_format, output_format,
                    p_names, p_descs, p_defs, p_count)
                parameters = {}
                for j in range(p_count[0]):
                    p_name = Library().string(p_names[0][j]).decode()
                    parameters[p_name] = TransformParameter(
                        Library().string(p_names[0][j]).decode(),
                        Library().string(p_descs[0][j]).decode(),
                        Library().string(p_defs[0][j]).decode())
                self.transforms[transform_name] = Transform(
                    transform_name,
                    Library().string(description[0]).decode(), parameters,
                    Library().string(input_format[0]).decode(),
                    Library().string(output_format[0]).decode())
                Library().destroy_transform_details(
                    description[0], input_format[0], output_format[0],
                    p_names[0], p_descs[0], p_defs[0], p_count[0])
            Library().destroy_transforms_list(names[0], list_size[0])
            self.logger.debug("Done with getting transforms")
            input_formats = Library().new("char***")
            output_formats = Library().new("char***")
            self.logger.debug("query_format_converters_list()")
            Library().query_format_converters_list(
                input_formats, output_formats, list_size)
            self.logger.debug("Got the list of %d format converters",
                              list_size[0])
            self.format_converters = collections.defaultdict(list)
            for i in range(list_size[0]):
                self.format_converters[
                    Library().string(input_formats[0][i]).decode()] \
                    .append(Library().string(output_formats[0][i]).decode())
            Library().destroy_format_converters_list(
                input_formats[0], output_formats[0], list_size[0])
            self.logger.debug("Done with getting format converters")

    def __new__(cls):
        if not cls._instance:
            cls._instance = super(Explorer, cls).__new__(cls)
        return cls._instance

    _instance = None
    transforms = None
