from SDLCreator import SDLCreator
from SDLExecutor import SDLExecutor

import xml.etree.ElementTree as ElementTree


class SDLInterface:

	CORE_CATEGORIES = [
		"camera",
		"film",
		"estimator",
		"sample-generator",
		"renderer",
		"option"
	]

	WORLD_CATEGORIES = [
		"geometry",
		"material",
		"motion",
		"light-source",
		"actor",
		"image",
		"frame-processor"
	]

	def __init__(self, root_element: ElementTree):

		self.category_name = ""
		self.type_name = ""
		self.extended_target = ""
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
			elif element.tag == "extend":
				self.extended_target = element.text.strip()
			elif element.tag == "name":
				self.name = element.text.strip()
			elif element.tag == "description":
				processed_text = element.text.split()
				processed_text = " ".join(processed_text)
				self.description = processed_text
			elif element.tag == "command":
				if element.attrib["type"] == "creator":
					self.creator = SDLCreator(element)
				elif element.attrib["type"] == "executor":
					self.executors.append(SDLExecutor(element))
			else:
				print("note: tag %s ignored" % element.tag)

	def has_creator(self):
		return self.creator is not None

	def has_executor(self):
		return self.executors

	def is_core(self):
		for category in SDLInterface.CORE_CATEGORIES:
			if self.category_name == category:
				return True
		return False

	def is_world(self):
		for category in SDLInterface.WORLD_CATEGORIES:
			if self.category_name == category:
				return True
		return False

	def is_extending(self):
		return self.extended_target

	def get_extended_category_name(self):
		return self.extended_target.split(".")[0]

	def get_extended_type_name(self):
		return self.extended_target.split(".")[1]

	def get_extended_full_type_name(self):
		category_name = self.get_extended_category_name()
		type_name = self.get_extended_type_name()
		return category_name + "(" + type_name + ")"

	def extend(self, other_interface: "SDLInterface"):

		if other_interface.creator is not None:
			for creator_input in other_interface.creator.inputs:
				self.creator.add_input(creator_input)

		self.executors.extend(other_interface.executors)

	def get_full_type_name(self):
		return self.category_name + "(" + self.type_name + ")"

	def __str__(self):

		result = "SDL Interface \n"

		result += "Category Name: " + self.category_name + "\n"
		result += "Type Name: " + self.type_name + "\n"
		result += "Extended Target: " + self.extended_target + "\n"
		result += "Name: " + self.name + "\n"
		result += "Description: " + self.description + "\n"

		if self.has_creator():
			result += str(self.creator)

		for executor in self.executors:
			result += str(executor)

		return result
