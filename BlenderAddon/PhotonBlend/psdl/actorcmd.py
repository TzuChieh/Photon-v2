from .cmd import CreationCommand, FunctionCommand
from .clause import SdlResourceIdentifierClause
from .clause import SdlReferenceClause
from .clause import ColorClause
from .clause import FloatClause
from .clause import StringClause
from .clause import Vector3Clause

from abc import abstractmethod


class ActorCreator(CreationCommand):

	@abstractmethod
	def __init__(self):
		super().__init__()

	@abstractmethod
	def get_type_name(self):
		return "actor"

	def get_type_category(self):
		return "actor"


class DomeCreator(ActorCreator):

	def __init__(self):
		super().__init__()

	def get_type_name(self):
		return "dome"

	def set_envmap_sdlri(self, sdlri):
		self.update_clause(SdlResourceIdentifierClause().set_name("env-map").set_data(sdlri))
