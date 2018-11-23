from SDLInput import SDLInput

import xml.etree.ElementTree as ElementTree


class SDLCreator:

	def __init__(self, root_element: ElementTree):
		self.inputs = []

		if root_element.tag != "command":
			print("warning: invalid SDL creator root element detected: %s" % root_element.tag)
			return

		for element in root_element:
			if element.tag == "input":
				self.inputs.append(SDLInput(element))

	def __str__(self):

		result = "SDL Creator \n"

		for sdl_input in self.inputs:
			result += str(sdl_input)

		return result
