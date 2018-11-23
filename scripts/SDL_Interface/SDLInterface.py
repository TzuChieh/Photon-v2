from SDLCreator import SDLCreator
from SDLExecutor import SDLExecutor

import xml.etree.ElementTree as ElementTree


class SDLInterface:

	def __init__(self, root_element: ElementTree):

		self.category_name = ""
		self.type_name = ""
		self.inherited_target = ""
		self.name = ""
		self.description = ""
		self.creator = None
		self.executors = []

		if root_element.tag != "SDL_interface":
			print("warning: invalid SDL interface root element detected: %s" % root_element.tag)
			return

		for element in root_element:
			if element.tag == "category":
				self.category_name = element.text.strip()
			elif element.tag == "type_name":
				self.type_name = element.text.strip()
			elif element.tag == "inherit":
				self.inherited_target = element.text.strip()
			elif element.tag == "name":
				self.name = element.text.strip()
			elif element.tag == "description":
				processed_text = element.text.strip()
				processed_text.replace("\n", "")
				processed_text.replace("\r", "")
				self.description = processed_text
			elif element.tag == "command":
				if element.attrib["type"] == "creator":
					self.creator = SDLCreator(element)
				elif element.attrib["type"] == "executor":
					self.executors.append(SDLExecutor(element))

	def has_creator(self):
		return self.creator is not None

	def has_executor(self):
		return self.executors

	def __str__(self):

		result = "SDL Interface \n"

		result += "Category Name: " + self.category_name + "\n"
		result += "Type Name: " + self.type_name + "\n"
		result += "Inherited Target: " + self.inherited_target + "\n"
		result += "Name: " + self.name + "\n"
		result += "Description: " + self.description + "\n"

		if self.has_creator():
			result += str(self.creator)

		for executor in self.executors:
			result += str(executor)

		return result
