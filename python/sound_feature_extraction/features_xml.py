"""
Created on Oct 3, 2013

@author: Vadim Markovtsev <v.markovtsev@samsung.com>
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
            ret += "(" + node.attrib["parameters"] + ")"
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
                features.append(child.attrib["name"] + " [" +
                                ','.join(path) + "]")
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
