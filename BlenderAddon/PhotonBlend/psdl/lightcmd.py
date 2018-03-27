from .cmd import CreationCommand
from .clause import ColorClause
from .clause import FloatClause

import mathutils

from abc import abstractmethod


class LightCreator(CreationCommand):

	@abstractmethod
	def __init__(self):
		super(LightCreator, self).__init__()

	@abstractmethod
	def get_type_name(self):
		return "light-source"

	def get_type_category(self):
		return "light-source"


class AreaLightCreator(LightCreator):

	def __init__(self):
		super(AreaLightCreator, self).__init__()

	def get_type_name(self):
		return "area"

	# TODO


class PointLightCreator(LightCreator):

	def __init__(self):
		super(PointLightCreator, self).__init__()

	def get_type_name(self):
		return "point"

	def set_linear_srgb_color(self, color):
		self.update_clause(ColorClause().set_name("linear-srgb").set_data(color))

	def set_watts(self, watts):
		self.update_clause(FloatClause().set_name("watts").set_data(watts))
