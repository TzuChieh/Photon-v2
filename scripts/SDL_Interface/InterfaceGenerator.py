from SDLInterface import SDLInterface
from SDLStruct import SDLStruct

from abc import ABC, abstractmethod
import copy


class InterfaceGenerator(ABC):
	def __init__(self):
		self.interfaces = []
		self.structs = []

	@abstractmethod
	def generate(self, output_directory):
		pass

	@abstractmethod
	def name(self):
		pass

	def add_interface(self, sdl_interface: SDLInterface):
		self.interfaces.append(copy.deepcopy(sdl_interface))

	def add_struct(self, sdl_struct: SDLStruct):
		self.structs.append(copy.deepcopy(sdl_struct))

	def resolve_interface_extension(self):
		# Start by dividing interfaces into resolved and unresolved

		resolved_interfaces = {}
		unresolved_interfaces = []
		for interface in self.interfaces:
			if interface.is_soft_extending():
				unresolved_interfaces.append(interface)
			else:
				resolved_interfaces[interface.get_full_type_name()] = interface

		# Iteratively hard extend unresolved interfaces with resolved ones

		while unresolved_interfaces:
			has_progress = False
			for interface in unresolved_interfaces:
				target_name = interface.get_soft_extended_full_type_name()
				extended_interface = resolved_interfaces.get(target_name, None)
				if extended_interface is None:
					continue
				else:
					interface.hard_extend(extended_interface)
					interface.clear_soft_extended_target()
					resolved_interfaces[interface.get_full_type_name()] = interface
					unresolved_interfaces.remove(interface)
					has_progress = True

			if not has_progress:
				return False

		# HACK: expanding SDL struct inputs as structs is not supported yet

		for sdl_interface in self.interfaces:
			if sdl_interface.has_creator():
				sdl_creator = sdl_interface.creator
				i = 0
				while i < len(sdl_creator.inputs):
					sdl_input = sdl_creator.inputs[i]
					if sdl_input.is_struct():
						for sdl_struct in self.structs:
							if sdl_struct.type_name == sdl_input.type_name:
								sdl_creator.inputs.extend(sdl_struct.inputs)

					i += 1

			if sdl_interface.has_executor():
				for sdl_executor in sdl_interface.executors:
					i = 0
					while i < len(sdl_executor.inputs):
						sdl_input = sdl_executor.inputs[i]
						if sdl_input.is_struct():
							for sdl_struct in self.structs:
								if sdl_struct.type_name == sdl_input.type_name:
									sdl_executor.inputs.extend(sdl_struct.inputs)

						i += 1

		return True
