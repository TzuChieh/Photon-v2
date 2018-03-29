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

	@abstractmethod
	def get_type_name(self):
		return "area"

	def set_linear_srgb_color(self, color):
		self.update_clause(ColorClause().set_name("linear-srgb").set_data(color))

	def set_watts(self, watts):
		self.update_clause(FloatClause().set_name("watts").set_data(watts))


class PointLightCreator(AreaLightCreator):

	def __init__(self):
		super(PointLightCreator, self).__init__()

	def get_type_name(self):
		return "point"


class SphereLightCreator(AreaLightCreator):

	def __init__(self):
		super(SphereLightCreator, self).__init__()

	def get_type_name(self):
		return "sphere"

	def set_radius(self, radius):
		self.update_clause(FloatClause().set_name("radius").set_data(radius))


class RectangleLightCreator(AreaLightCreator):

	def __init__(self):
		super(RectangleLightCreator, self).__init__()

	def get_type_name(self):
		return "rectangle"

	def set_width(self, width):
		self.update_clause(FloatClause().set_name("width").set_data(width))

	def set_height(self, height):
		self.update_clause(FloatClause().set_name("height").set_data(height))

