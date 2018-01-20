from .. import utility


class SdlConsole:

	def __init__(self, working_directory):
		self.__working_directory = working_directory

	def get_working_directory(self):
		return self.__working_directory

	def create_resource_folder(self, sdl_resource_identifier):

		res_path        = utility.get_appended_path(self.__working_directory, sdl_resource_identifier.get_path())
		res_folder_path = utility.get_folder_path(res_path)
		utility.create_folder(res_folder_path)




