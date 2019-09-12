from SDLInterface import SDLInterface

import xml.etree.ElementTree as ElementTree


class InterfaceParser:
    BEGIN_TAG = "<SDL_interface>"
    END_TAG = "</SDL_interface>"

    def __init__(self, raw_string):
        self.interface = None

        begin_index = raw_string.find(InterfaceParser.BEGIN_TAG)
        end_index = raw_string.rfind(InterfaceParser.END_TAG)
        if begin_index == -1 or end_index == -1 or end_index <= begin_index:
            return

        interface_string = raw_string[begin_index:end_index+len(InterfaceParser.END_TAG)]
        self.interface = SDLInterface(ElementTree.fromstring(interface_string))

    def has_interface(self):
        return self.interface is not None
