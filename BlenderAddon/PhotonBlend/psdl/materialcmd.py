from .cmd import CreationCommand, FunctionCommand
from .clause import ColorClause
from .clause import FloatClause
from .clause import StringClause

import mathutils

from abc import abstractmethod


class MaterialCreator(CreationCommand):

	@abstractmethod
	def __init__(self):
		super(MaterialCreator, self).__init__()

	@abstractmethod
	def get_type_name(self):
		return "material"

	def get_type_category(self):
		return "material"


class MatteOpaqueCreator(MaterialCreator):

	def __init__(self):
		super(MatteOpaqueCreator, self).__init__()

	def get_type_name(self):
		return "matte-opaque"

	def set_albedo(self, albedo):
		self.update_clause(ColorClause().set_name("albedo").set_data(albedo))


class AbstractAbradedCreator(MaterialCreator):

	@abstractmethod
	def __init__(self):
		super(AbstractAbradedCreator, self).__init__()

	def set_albedo(self, albedo):
		self.update_clause(ColorClause().set_name("albedo").set_data(albedo))

	def set_f0(self, f0):
		self.update_clause(ColorClause().set_name("f0").set_data(f0))

	def set_roughness(self, roughness):
		self.update_clause(FloatClause().set_name("roughness").set_data(roughness))


class AbradedOpaqueCreator(AbstractAbradedCreator):

	def __init__(self):
		super(AbradedOpaqueCreator, self).__init__()

	def get_type_name(self):
		return "abraded-opaque"

	def set_anisotropicity(self, is_anisopropic):
		if is_anisopropic:
			self.update_clause(StringClause().set_name("type").set_data("aniso-metallic-ggx"))
		else:
			self.update_clause(StringClause().set_name("type").set_data("iso-metallic-ggx"))

	def set_roughness_u(self, roughness_u):
		self.update_clause(FloatClause().set_name("roughness-u").set_data(roughness_u))

	def set_roughness_v(self, roughness_v):
		self.update_clause(FloatClause().set_name("roughness-v").set_data(roughness_v))


class AbradedTranslucentCreator(AbstractAbradedCreator):

	def __init__(self):
		super(AbradedTranslucentCreator, self).__init__()

	def get_type_name(self):
		return "abraded-translucent"

	def set_ior(self, ior):
		self.update_clause(FloatClause().set_name("ior").set_data(ior))


