from SDLInput import SDLInput

import xml.etree.ElementTree as ElementTree


class SDLStruct:
    def __init__(self, root_element: ElementTree):
        self.inputs = []
        self.type_name = ""

        for element in root_element:
            if element.tag == "input":
                self.add_input(SDLInput(element))
            elif element.tag == "type_name" and element.text is not None:
                self.type_name = element.text.strip()
            else:
                print("note: tag %s is not recognized, ignoring" % element.tag)

        # Sanity checks

        if not self.type_name:
            print("warning: SDL struct must have type name")

    def add_input(self, sdl_input: SDLInput):
        self.inputs.append(sdl_input)

    def __str__(self):
        result = "SDL Struct \n"

        result += "Type Name: " + self.type_name + "\n"

        for sdl_input in self.inputs:
            result += str(sdl_input)

        return result
