from .cmd import CreationCommand, FunctionCommand
from .clause import ColorClause
from .clause import FloatClause
from .clause import StringClause
from .clause import SdlResourceIdentifierClause
from .clause import SdlReferenceClause

import mathutils

from abc import abstractmethod


class MaterialCreator(CreationCommand):

	@abstractmethod
	def __init__(self):
		super().__init__()

	@abstractmethod
	def get_type_name(self):
		return "material"

	def get_type_category(self):
		return "material"


class MatteOpaqueCreator(MaterialCreator):

	def __init__(self):
		super().__init__()

	def get_type_name(self):
		return "matte-opaque"

	def set_albedo_color(self, albedo):
		self.update_clause(ColorClause().set_name("albedo").set_data(albedo))

	def set_albedo_image_sdlri(self, sdlri):
		self.update_clause(SdlResourceIdentifierClause().set_name("albedo").set_data(sdlri))

	def set_albedo_image_ref(self, ref_name):
		clause = SdlReferenceClause()
		clause.set_name("albedo")
		clause.set_data(ref_name)
		clause.set_reference_category("image")
		self.update_clause(clause)


class AbstractAbradedCreator(MaterialCreator):

	@abstractmethod
	def __init__(self):
		super().__init__()

	def set_albedo(self, albedo):
		self.update_clause(ColorClause().set_name("albedo").set_data(albedo))

	def set_f0(self, f0):
		self.update_clause(ColorClause().set_name("f0").set_data(f0))

	def set_roughness(self, roughness):
		self.update_clause(FloatClause().set_name("roughness").set_data(roughness))


class AbradedOpaqueCreator(AbstractAbradedCreator):

	def __init__(self):
		super().__init__()

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
		super().__init__()

	def get_type_name(self):
		return "abraded-translucent"

	def set_ior(self, ior):
		self.update_clause(FloatClause().set_name("ior").set_data(ior))


class FullCreator(MaterialCreator):

	def __init__(self):
		super().__init__()

	def get_type_name(self):
		return "full"

	def set_surface_ref(self, material_name):
		clause = SdlReferenceClause()
		clause.set_name("surface")
		clause.set_data(material_name)
		clause.set_reference_category("material")
		self.update_clause(clause)


class BinaryMixedSurfaceCreator(MaterialCreator):

	def __init__(self):
		super().__init__()

	def get_type_name(self):
		return "binary-mixed-surface"

	def set_surface_material0_ref(self, name):
		clause = SdlReferenceClause()
		clause.set_name("material-0")
		clause.set_data(name)
		clause.set_reference_category("material")
		self.update_clause(clause)

	def set_surface_material1_ref(self, name):
		clause = SdlReferenceClause()
		clause.set_name("material-1")
		clause.set_data(name)
		clause.set_reference_category("material")
		self.update_clause(clause)

	def set_float_factor(self, factor):
		self.update_clause(FloatClause().set_name("factor").set_data(factor))

