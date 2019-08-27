from .cmd import CreationCommand
from .clause import ColorClause
from .clause import FloatClause
from .clause import SdlResourceIdentifierClause
from .clause import SdlReferenceClause

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


class ModelLightCreator(LightCreator):

	def __init__(self):
		super(ModelLightCreator, self).__init__()

	def get_type_name(self):
		return "model"

	def set_emitted_radiance_linear_srgb(self, color):
		self.update_clause(ColorClause().set_name("emitted-radiance").set_data(color))

	def set_emitted_radiance_sdlri(self, sdlri):
		self.update_clause(SdlResourceIdentifierClause().set_name("emitted-radiance").set_data(sdlri))

	def set_emitted_radiance_image(self, image_name):
		ref_clause = SdlReferenceClause()
		ref_clause.set_reference_category("image")
		self.update_clause(ref_clause.set_name("emitted-radiance").set_data(image_name))

	def set_geometry(self, geometry_name):
		ref_clause = SdlReferenceClause()
		ref_clause.set_reference_category("geometry")
		self.update_clause(ref_clause.set_name("geometry").set_data(geometry_name))

	def set_material(self, material_name):
		ref_clause = SdlReferenceClause()
		ref_clause.set_reference_category("matl")
		self.update_clause(ref_clause.set_name("matl").set_data(material_name))
