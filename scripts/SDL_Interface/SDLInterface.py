from SDLCreator import SDLCreator
from SDLExecutor import SDLExecutor
import sdl_types

import xml.etree.ElementTree as ElementTree


class SDLInterface:
	"""
	Describes an interface in Photon that is controllable via scene description language.
	"""

	def __init__(self, root_element: ElementTree):
		self.category_name = ""
		self.type_name = ""
		self.soft_extended_target = ""
		self.name = ""
		self.description = ""
		self.creator = None
		self.executors = []

		if root_element.tag != "SDL_interface":
			print("warning: invalid SDL interface root element detected: %s" % root_element.tag)
			return

		for element in root_element:
			if element.tag == "category" and element.text is not None:
				self.category_name = element.text.strip()
			elif element.tag == "type_name" and element.text is not None:
				self.type_name = element.text.strip()
			elif element.tag == "extend" and element.text is not None:
				self.soft_extended_target = element.text.strip()
			elif element.tag == "name" and element.text is not None:
				self.name = element.text.strip()
			elif element.tag == "description" and element.text is not None:
				processed_text = element.text.split()
				processed_text = " ".join(processed_text)
				self.description = processed_text
			elif element.tag == "command":
				if element.attrib["type"] == "creator":
					if self.creator is not None:
						print("warning: overwriting previously defined creator (> 1 creator detected)")
					self.creator = SDLCreator(element)
				elif element.attrib["type"] == "executor":
					self.executors.append(SDLExecutor(element))
			else:
				print("note: tag %s is not recognized, ignoring" % element.tag)

	def has_creator(self):
		return self.creator is not None

	def has_executor(self):
		return self.executors

	def is_core(self):
		return sdl_types.is_core_category(self.category_name)

	def is_world(self):
		return sdl_types.is_world_category(self.category_name)

	def is_soft_extending(self):
		return self.soft_extended_target

	def get_soft_extended_category_name(self):
		return self.soft_extended_target.split(".")[0]

	def get_soft_extended_type_name(self):
		return self.soft_extended_target.split(".")[1]

	def get_soft_extended_full_type_name(self):
		category_name = self.get_soft_extended_category_name()
		type_name = self.get_soft_extended_type_name()
		return category_name + "(" + type_name + ")"

	def clear_soft_extended_target(self):
		self.soft_extended_target = ""

	def hard_extend(self, other_interface: 'SDLInterface'):
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
		result += "Extended Target: " + self.soft_extended_target + "\n"
		result += "Name: " + self.name + "\n"
		result += "Description: " + self.description + "\n"

		if self.has_creator():
			result += str(self.creator)

		for executor in self.executors:
			result += str(executor)

		return result
