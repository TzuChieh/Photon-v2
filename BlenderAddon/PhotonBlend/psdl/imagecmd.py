from .cmd import CreationCommand, FunctionCommand
from .clause import ColorClause
from .clause import FloatClause
from .clause import StringClause

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
		self.__image_data = None

	@abstractmethod
	def get_type_name(self):
		return "ldr-picture"

	def to_sdl(self):

		if self.__image_data is not None:
			# TODO: sotre image as file
			pass

		super(LdrPictureImageCreator, self).to_sdl()

	

