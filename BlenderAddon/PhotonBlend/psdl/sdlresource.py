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

	def get_identifier(self):
		res_path       = self.get_path()
		res_identifier = ""
		for ch in res_path:
			if ch != utility.path_separator():
				res_identifier += ch
			else:
				res_identifier += "/"
		return res_identifier

	def is_valid(self):
		return self.__resource_folder_path or self.__resource_filename

	def __str__(self):
		return ("resource folder path: " + self.__resource_folder_path + ", "
		        "resource filename: "    + self.__resource_filename)

	def __bool__(self):
		return self.is_valid()
	__nonzero__ = __bool__


def save_blender_image(b_image, image_sdlri, sdlconsole):
	sdlconsole.create_resource_folder(image_sdlri)
	image_path = utility.get_appended_path(sdlconsole.get_working_directory(), image_sdlri.get_path())
	b_image.save_render(image_path)
