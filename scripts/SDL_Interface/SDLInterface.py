from .SDLCreator import SDLCreator
from .SDLExecutor import SDLExecutor

import xml.etree.ElementTree as ElementTree


class SDLInterface:

	def __init__(self, root_element: ElementTree):

		self.category_name = ""
		self.type_name = ""
		self.inherited_target = ""
		self.name = ""
		self.description = ""
		self.creator = None
		self.executor = []

		if root_element.tag != "SDL_interface":
			print("warning: invalid SDL interface root element detected: %s" % root_element.tag)
			return

		# TODO: remove some spaces and newline chars
		for element in root_element:
			if element.tag == "category":
				self.category_name = element.text
			elif element.tag == "type_name":
				self.type_name = element.text
			elif element.tag == "inherit":
				self.inherited_target = element.text
			elif element.tag == "name":
				self.name = element.text
			elif element.tag == "description":
				self.description = element.text
			elif element.tag == "command":
				if element.attrib["type"] == "creator":
					self.creator = SDLCreator(element)
				elif element.attrib["type"] == "executor":
					self.executor = SDLExecutor(element)
