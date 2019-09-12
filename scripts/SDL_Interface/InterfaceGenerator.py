from SDLInterface import SDLInterface

from abc import ABC, abstractmethod
import copy


class InterfaceGenerator(ABC):
	def __init__(self):
		self.interfaces = []

	@abstractmethod
	def generate(self, output_directory):
		pass

	@abstractmethod
	def name(self):
		pass

	def add_interface(self, sdl_interface: SDLInterface):
		self.interfaces.append(copy.deepcopy(sdl_interface))

	def resolve_interface_extension(self):
		resolved_interfaces = {}
		unresolved_interfaces = []
		for interface in self.interfaces:
			if interface.is_extending():
				unresolved_interfaces.append(interface)
			else:
				resolved_interfaces[interface.get_full_type_name()] = interface

		while unresolved_interfaces:
			has_progress = False
			for interface in unresolved_interfaces:
				target_name = interface.get_extended_full_type_name()
				extended_interface = resolved_interfaces.get(target_name, None)
				if extended_interface is None:
					continue
				else:
					interface.extend(extended_interface)
					resolved_interfaces[interface.get_full_type_name()] = interface
					unresolved_interfaces.remove(interface)
					has_progress = True

			if not has_progress:
				return False

		return True
