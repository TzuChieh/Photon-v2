from SDLInput import SDLInput

import xml.etree.ElementTree as ElementTree


class SDLExecutor:
	def __init__(self, root_element: ElementTree):
		self.inputs = []
		self.name = root_element.attrib["name"]
		self.description = ""

		if root_element.tag != "command":
			print("warning: invalid SDL executor root element detected: %s" % root_element.tag)
			return

		for element in root_element:
			if element.tag == "input":
				self.add_input(SDLInput(element))
			elif element.tag == "description":
				processed_text = element.text.split()
				processed_text = " ".join(processed_text)
				self.description = processed_text
			else:
				print("note: tag %s ignored" % element.tag)

	def add_input(self, sdl_input: SDLInput):
		self.inputs.append(sdl_input)

	def __str__(self):
		result = "SDL Executor \n"

		result += "Name: " + self.name + "\n"
		result += "Description: " + self.description + "\n"

		for sdl_input in self.inputs:
			result += str(sdl_input)

		return result
