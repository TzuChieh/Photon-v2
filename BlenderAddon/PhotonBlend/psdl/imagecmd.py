from .cmd import CreationCommand, FunctionCommand
from .clause import SdlResourceIdentifierClause
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

	def get_type_name(self):
		return "ldr-picture"

	def set_image_sdlri(self, sdlri):
		self.update_clause(SdlResourceIdentifierClause().set_name("image").set_data(sdlri))

	

