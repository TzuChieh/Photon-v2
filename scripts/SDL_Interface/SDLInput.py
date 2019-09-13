import sdl_types

import xml.etree.ElementTree as ElementTree


class SDLInput:
	def __init__(self, root_element: ElementTree):
		self.name = ""
		self.type_name = ""
		self.description = ""

		if root_element.tag != "input":
			print("warning: invalid SDL input root element detected: %s" % root_element.tag)
			return

		self.name = root_element.attrib["name"]
		self.type_name = root_element.attrib["type"]

		# TODO: remove spaces and some newline chars
		for element in root_element:
			if element.tag == "description":
				processed_text = element.text.strip()
				processed_text.replace("\n", "")
				processed_text.replace("\r", "")
				self.description = processed_text

	def is_value(self):
		return sdl_types.is_value(self.type_name)

	def is_reference(self):
		return sdl_types.is_reference(self.type_name)

	def is_struct(self):
		return sdl_types.is_struct(self.type_name)

	def __str__(self):
		result = "SDL Input \n"

		result += "Name: " + self.name + "\n"
		result += "Type Name: " + self.type_name + "\n"
		result += "Description: " + self.description + "\n"

		return result
