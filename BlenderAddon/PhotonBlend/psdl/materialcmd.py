from .cmd import CreationCommand, FunctionCommand
from .clause import ColorClause
from .clause import FloatClause
from .clause import StringClause


class Material:
	_category_name = "material"
	_type_name    = "material"


class MatteOpaque(Material):
	_type_name = "matte-opaque"

	@classmethod
	def create(cls, name, albedo):
		command = CreationCommand()
		command.set_type_info(cls._category_name, cls._type_name)
		command.set_data_name(name)
		command.add_clause(ColorClause().set_name("albedo").set_data(albedo))
		return command


class AbradedOpaque(Material):
	_type_name = "abraded-opaque"

	@classmethod
	def create(cls, **kwargs):

		command = CreationCommand()
		command.set_type_info(cls._category_name, cls._type_name)
		command.set_data_name(kwargs["name"])

		command.add_clause(ColorClause().set_name("albedo").set_data(kwargs["albedo"]))
		command.add_clause(ColorClause().set_name("f0").set_data(kwargs["f0"]))
		if not kwargs["is_anisotropic"]:
			command.add_clause(StringClause().set_name("type").set_data("iso-metallic-ggx"))
			command.add_clause(FloatClause().set_name("roughness").set_data(kwargs["roughness"]))
		else:
			command.add_clause(StringClause().set_name("type").set_data("aniso-metallic-ggx"))
			command.add_clause(FloatClause().set_name("roughness-u").set_data(kwargs["roughness_u"]))
			command.add_clause(FloatClause().set_name("roughness-v").set_data(kwargs["roughness_v"]))

		return command



