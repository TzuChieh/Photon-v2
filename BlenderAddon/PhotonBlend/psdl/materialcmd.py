from .cmd import CreationCommand, FunctionCommand
from .clause import ColorClause


class Material:
	_categoryName = "material"


class MatteOpaque(Material):
	_typeName = "matte-opaque"

	@classmethod
	def create(cls, name, albedo):
		command = CreationCommand()
		command.set_type_info(cls._categoryName, cls._typeName)
		command.set_data_name(name)
		command.add_clause(ColorClause().set_name("albedo").set_data(albedo))
		return command



