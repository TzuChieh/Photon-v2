from .. import utility


class SdlResourceIdentifier:

	def __init__(self):
		self.__resource_folder_path = ""
		self.__resource_filename    = ""

	def append_folder(self, folder_name):
		self.__resource_folder_path = utility.get_appended_path(self.__resource_folder_path, folder_name)
		self.__resource_folder_path += utility.path_separator()

	def set_file(self, filename):
		self.__resource_filename = filename

	def get_path(self):
		return self.__resource_folder_path + self.__resource_filename

	def is_valid(self):
		return self.__resource_folder_path or self.__resource_filename

	def __str__(self):
		return ("resource folder path: " + self.__resource_folder_path + ", "
		        "resource filename: "    + self.__resource_filename)
