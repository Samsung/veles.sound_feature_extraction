"""
Created on Oct 3, 2013

@author: Vadim Markovtsev <v.markovtsev@samsung.com>

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


from xml.etree import ElementTree


class FeaturesXml(object):
    """
    Parses the specified XML file with descriptions of sound features
    to extract.
    """

    @staticmethod
    def node_to_str(node):
        ret = node.attrib["name"]
        if "parameters" in node.attrib:
            ret += "(%s)" % node.attrib["parameters"]
        if "condition" in node.attrib:
            ret += "{%s}" % node.attrib["condition"]
        return ret

    @staticmethod
    def add_children(node, path):
        features = []
        for child in node:
            if child.tag == "transform":
                inner_path = list(path)
                inner_path.append(FeaturesXml.node_to_str(child))
                features += FeaturesXml.add_children(child, inner_path)
            if child.tag == "feature":
                features.append("%s [%s]" % (
                    child.attrib["name"], ','.join(path)))
        return features

    @staticmethod
    def parse(file_name):
        """
        Parses the specified XML file with descriptions of sound features,
        returning the list of strings, each describing a distinct feature.
        """
        tree = ElementTree.parse(file_name)
        root = tree.getroot()
        return FeaturesXml.add_children(root, "")
