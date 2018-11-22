import xml.etree.ElementTree as ElementTree


class SDLExecutor:
	def __init__(self, root_element: ElementTree):
		self.inputs = []
		self.name  = ""
		self.description = ""
