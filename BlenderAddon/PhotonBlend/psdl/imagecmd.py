from .cmd import CreationCommand, FunctionCommand
from .clause import SdlResourceIdentifierClause
from .clause import SdlReferenceClause
from .clause import ColorClause
from .clause import FloatClause
from .clause import StringClause
from .clause import Vector3Clause
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


class RealMathImageCreator(ImageCreator):

	def __init__(self):
		super(RealMathImageCreator, self).__init__()

	def get_type_name(self):
		return "real-math"

	def set_operand_image(self, image_name):
		clause = SdlReferenceClause()
		clause.set_reference_category("image")
		clause.set_name("operand")
		clause.set_data(image_name)
		self.update_clause(clause)

	def set_real_value(self, float_value):
		self.update_clause(FloatClause().set_name("value").set_data(float_value))

	def set_math_op(self, math_op_name):
		self.update_clause(StringClause().set_name("math-op").set_data(math_op_name))

	def set_multiply(self):
		self.set_math_op("multiply")

	def set_add(self):
		self.set_math_op("add")


# TODO: value type
class ConstantImageCreator(ImageCreator):

	def __init__(self):
		super(ConstantImageCreator, self).__init__()

	def get_type_name(self):
		return "constant"

	def set_real_value(self, float_value):
		self.update_clause(FloatClause().set_name("value").set_data(float_value))

	def set_vec3_value(self, vec3):
		self.update_clause(Vector3Clause().set_name("value").set_data(vec3))

	def set_rgb_value(self, rgb):
		self.update_clause(ColorClause().set_name("value").set_data(rgb))


	

