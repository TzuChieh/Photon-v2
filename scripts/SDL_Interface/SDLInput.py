import xml.etree.ElementTree as ElementTree


class SDLInput:
	def __init__(self, root_element: ElementTree):
		self.name = ""
		self.type_name = ""
		self.description = ""

		if root_element.tag != "input":
			print("warning: invalid SDL input root element detected: %s" % root_element.tag)
			return

		self.name = root_element.get("name")
		self.type_name = root_element.get("type")

		# TODO: remove spaces and some newline chars
		for element in root_element:
			if element.tag == "description":
				self.description = element.text
