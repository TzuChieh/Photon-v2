from SDLInterface import SDLInterface
from SDLStruct import SDLStruct

import xml.etree.ElementTree as ElementTree


# Find a root element tree from the string given
def find_element_tree(raw_string, begin_tag, end_tag):
    begin_index = raw_string.find(begin_tag)
    end_index = raw_string.rfind(end_tag)
    if begin_index == -1 or end_index == -1 or end_index <= begin_index:
        return None

    interface_string = raw_string[begin_index:end_index + len(end_tag)]
    return ElementTree.fromstring(interface_string)


class InterfaceParser:
    INTERFACE_BEGIN_TAG = '<SDL_interface>'
    INTERFACE_END_TAG = '</SDL_interface>'
    STRUCT_BEGIN_TAG = '<SDL_struct>'
    STRUCT_END_TAG = '</SDL_struct>'

    def __init__(self, raw_string):
        self.interface = None
        self.struct = None

        # Find and parse interface
        interface_element_tree = find_element_tree(
                raw_string,
                InterfaceParser.INTERFACE_BEGIN_TAG,
                InterfaceParser.INTERFACE_END_TAG)
        if interface_element_tree is not None:
            self.interface = SDLInterface(interface_element_tree)

        # Find and parse struct
        struct_element_tree = find_element_tree(
            raw_string,
            InterfaceParser.STRUCT_BEGIN_TAG,
            InterfaceParser.STRUCT_END_TAG)
        if struct_element_tree is not None:
            self.struct = SDLStruct(struct_element_tree)

    def has_interface(self):
        return self.interface is not None

    def has_struct(self):
        return self.struct is not None
