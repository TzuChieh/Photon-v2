from .cmd import CreationCommand, FunctionCommand
from .clause import ColorClause
from .clause import FloatClause
from .clause import StringClause
from .. import utility
from . import sdlresource

from abc import abstractmethod


class ImageCreator(CreationCommand):

	@abstractmethod
	def __init__(self):
		super(ImageCreator, self).__init__()

	@abstractmethod
	def get_type_name(self):
		return "image"

	def get_type_category(self):
		return "image"


class LdrPictureImageCreator(ImageCreator):

	def __init__(self):
		super(LdrPictureImageCreator, self).__init__()
		self.__b_image_data = None
		self.__image_sdlri  = None

	@abstractmethod
	def get_type_name(self):
		return "ldr-picture"

	def set_image_data(self, b_image_data):
		self.__b_image_data = b_image_data

	def set_image_sdlri(self, sdlri):
		self.__image_sdlri = sdlri

	def to_sdl(self, sdlconsole):

		if self.__b_image_data is not None:
			sdlresource.save_blender_image(self.__b_image_data, self.__image_sdlri, sdlconsole)

		super(LdrPictureImageCreator, self).to_sdl(sdlconsole)

	

