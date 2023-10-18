import utility

import shutil
import os.path
from collections import deque
from pathlib import PureWindowsPath


class SdlCommandQueue:

	def __init__(self):
		self.__commands = deque()

	def to_sdl(self):
		strings = []
		for command in self.__commands:
			strings.append(command.to_sdl())

		return "".join(strings)

	def queue_command(self, command):
		self.__commands.append(command)

	def queue_commands(self, command_queue):
		for command in command_queue.__commands:
			self.queue_command(command)

	def clear(self):
		self.__commands.clear()

	def pop_command(self):
		if len(self.__commands) != 0:
			return self.__commands.popleft()
		else:
			return None


class SdlConsole:

	def __init__(self, working_directory, scene_name):
		self.__working_directory = working_directory
		self.__command_queue     = SdlCommandQueue()
		self.__command_filename  = scene_name + ".p2"
		self.__command_file      = None

	def start(self):
		command_file_path = utility.get_appended_path(self.__working_directory, self.__command_filename)
		self.__command_file = open(command_file_path, "w", encoding = "utf-8")

	def finish(self):
		self.write_queued_commands()
		self.__command_file.close()

	def get_working_directory(self):
		return self.__working_directory

	def create_resource_folder(self, sdl_resource_identifier):

		if not sdl_resource_identifier.is_valid():
			print("SDL resource identifier is invalid: %s" % sdl_resource_identifier)
			return

		res_path        = utility.get_appended_path(self.__working_directory, sdl_resource_identifier.get_path())
		res_folder_path = utility.get_folder_path(res_path)
		utility.create_folder(res_folder_path)

	def bundle_file(self, src_file_path, bundled_folder_path):
		# Create the bundled folder
		dst_folder_path = utility.get_appended_path(self.get_working_directory(), bundled_folder_path)
		utility.create_folder(dst_folder_path)
		
		# Copy source file to the bundled folder
		dst_file_path = utility.get_appended_path(dst_folder_path, utility.get_filename(src_file_path))
		shutil.copyfile(src_file_path, dst_file_path)

		bundled_file_path = utility.get_appended_path(bundled_folder_path, utility.get_filename(src_file_path))
		
		# Make Windows path more cross-platform (basically removing backslashes)
		if os.path.sep == '\\':
			bundled_file_path = PureWindowsPath(bundled_file_path).as_posix()

		return bundled_file_path

	def queue_command(self, command):
		self.__command_queue.queue_command(command)

	def write_queued_commands(self):
		command = self.__command_queue.pop_command()
		while command is not None:
			self.__command_file.write(command.generate())
			command = self.__command_queue.pop_command()




