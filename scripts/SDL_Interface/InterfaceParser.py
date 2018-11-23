from SDLInterface import SDLInterface

import xml.etree.ElementTree as ElementTree


class InterfaceParser:

	begin_tag = "<SDL_interface>"
	end_tag = "</SDL_interface>"

	def __init__(self, raw_string):

		self.interface = None

		begin_index = raw_string.find(InterfaceParser.begin_tag)
		end_index = raw_string.rfind(InterfaceParser.end_tag)
		if begin_index == -1 or end_index == -1 or end_index <= begin_index:
			return

		interface_string = raw_string[begin_index:end_index+len(InterfaceParser.end_tag)]
		self.interface = SDLInterface(ElementTree.fromstring(interface_string))

	def has_interface(self):
		return self.interface is not None
