# ========================================
# NOTE: THIS FILE CONTAINS GENERATED CODE
#       DO NOT MODIFY
# ========================================
# last generated: 2018-11-28 02:25:27.596236

from abc import ABC, abstractmethod


class SDLData(ABC):

	def __init__(self):
		super().__init__()

	@abstractmethod
	def get_type(self):
		pass

	@abstractmethod
	def generate_data(self):
		pass


class SDLString(SDLData):

	def __init__(self, string=""):
		super().__init__()
		self.string = string

	def get_type(self):
		return "string"

	def generate_data(self):
		return self.string


class SDLInteger(SDLData):

	def __init__(self, integer=0):
		super().__init__()
		self.integer = integer

	def get_type(self):
		return "integer"

	def generate_data(self):
		return str(self.integer)


class SDLReal(SDLData):

	def __init__(self, real=0):
		super().__init__()
		self.real = real

	def get_type(self):
		return "real"

	def generate_data(self):
		return str(self.real)


class SDLVector3(SDLData):

	def __init__(self, vector=(0, 0, 0)):
		super().__init__()
		self.vector = vector

	def get_type(self):
		return "vector3"

	def generate_data(self):
		return "\"%f %f %f\"" % (self.vector[0], self.vector[1], self.vector[2])


class SDLQuaternion(SDLData):
	def __init__(self, quaternion=(0, 0, 0, 0)):
		super().__init__()
		self.quaternion = quaternion

	def get_type(self):
		return "quaternion"

	def generate_data(self):
		return "\"%f %f %f %f\"" % (self.quaternion[0], self.quaternion[1], self.quaternion[2], self.quaternion[3])


class SDLRealArray(SDLData):

	def __init__(self, array=None):
		super().__init__()
		if array is None:
			self.array = []
		else:
			self.array = array

	def get_type(self):
		return "real-array"

	def generate_data(self):
		fragments = ["{"]
		for real in self.array:
			fragments.append("%f " % real)
		fragments.append("}")
		return "".join(fragments)

	def set_array(self, array):
		self.array = array
		return self

	def add(self, real):
		self.array.append(real)
		return self


class SDLVector3Array(SDLData):

	def __init__(self, array=None):
		super().__init__()
		if array is None:
			self.array = []
		else:
			self.array = array

	def get_type(self):
		return "vector3-array"

	def generate_data(self):
		fragments = ["{"]
		for vector3 in self.array:
			fragments.append("\"%f %f %f\" " % (vector3[0], vector3[1], vector3[2]))
		fragments.append("}")
		return "".join(fragments)

	def set_array(self, array):
		self.array = array
		return self

	def add(self, vector3):
		self.array.append(vector3)
		return self


# TODO: make a reference type for each category
class SDLReference(SDLData):
	def __init__(self, ref_type="", ref_name=""):
		super().__init__()
		self.ref_type = ref_type
		self.ref_name = ref_name

	def get_type(self):
		return self.ref_type

	def generate_data(self):
		return "\"@%s\"" % self.ref_name


class SDLRaw(SDLData):

	def __init__(self, type_string="", data_string=""):
		super().__init__()
		self.type_string = type_string
		self.data_string = data_string

	def get_type(self):
		return self.type_string

	def generate_data(self):
		return self.data_string


class SDLCommand(ABC):

	def __init__(self):
		super().__init__()
		self._inputs = []

	@abstractmethod
	def get_prefix(self):
		pass

	@abstractmethod
	def get_full_type(self):
		pass

	@abstractmethod
	def generate(self):
		pass

	def set_input(self, name, data: SDLData):
		self._inputs.append((name, data))

	def _generate_input_fragments(self, out_fragments):
		for name, data in self._inputs:
			out_fragments.append("[")
			out_fragments.append(data.get_type() + " ")
			out_fragments.append(name + " ")
			out_fragments.append(data.generate_data())
			out_fragments.append("]")


class SDLCreatorCommand(SDLCommand):

	def __init__(self):
		super().__init__()
		self.__data_name = ""

	@abstractmethod
	def get_full_type(self):
		pass

	def get_prefix(self):
		return "->"

	def generate(self):

		# TODO: some part can be pre-generated
		fragments = [
			self.get_prefix(), " ",
			self.get_full_type(), " ",
			"\"@" + self.__data_name + "\"", " "]
		self._generate_input_fragments(fragments)
		fragments.append("\n")

		return "".join(fragments)

	def set_data_name(self, data_name):
		self.__data_name = data_name


class SDLExecutorCommand(SDLCommand):

	def __init__(self):
		super().__init__()
		self.__target_name = ""
		self.__inputs = []

	@abstractmethod
	def get_full_type(self):
		pass

	@abstractmethod
	def get_name(self):
		pass

	def get_prefix(self):
		return "->"

	def generate(self):

		# TODO: some part can be pre-generated
		fragments = [
			self.get_prefix(), " ",
			self.get_full_type(), " ",
			self.get_name(), "(",
			"\"@" + self.__target_name + "\")", " "]
		self._generate_input_fragments(fragments)
		fragments.append("\n")

		return "".join(fragments)

	def set_target_name(self, data_name):
		self.__target_name = data_name


class SDLCoreCommand(SDLCommand):

	def __init__(self):
		super().__init__()

	@abstractmethod
	def get_full_type(self):
		pass

	def get_prefix(self):
		return "##"

	def generate(self):

		fragments = [
			self.get_prefix(), " ",
			self.get_full_type(), " "]
		self._generate_input_fragments(fragments)
		fragments.append("\n")

		return "".join(fragments)


class LightActorCreator(SDLCreatorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "actor(light)"

    def set_light_source(self, light_source: SDLData):
        self.set_input("light-source", light_source)


class LightActorTranslate(SDLExecutorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "actor(light)"

    def get_name(self):
        return "translate"

    def set_factor(self, factor: SDLData):
        self.set_input("factor", factor)


class LightActorRotate(SDLExecutorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "actor(light)"

    def get_name(self):
        return "rotate"

    def set_axis(self, axis: SDLData):
        self.set_input("axis", axis)

    def set_degree(self, degree: SDLData):
        self.set_input("degree", degree)

    def set_factor(self, factor: SDLData):
        self.set_input("factor", factor)


class LightActorScale(SDLExecutorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "actor(light)"

    def get_name(self):
        return "scale"

    def set_factor(self, factor: SDLData):
        self.set_input("factor", factor)


class ModelActorCreator(SDLCreatorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "actor(model)"

    def set_geometry(self, geometry: SDLData):
        self.set_input("geometry", geometry)

    def set_material(self, material: SDLData):
        self.set_input("material", material)


class ModelActorTranslate(SDLExecutorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "actor(model)"

    def get_name(self):
        return "translate"

    def set_factor(self, factor: SDLData):
        self.set_input("factor", factor)


class ModelActorRotate(SDLExecutorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "actor(model)"

    def get_name(self):
        return "rotate"

    def set_axis(self, axis: SDLData):
        self.set_input("axis", axis)

    def set_degree(self, degree: SDLData):
        self.set_input("degree", degree)

    def set_factor(self, factor: SDLData):
        self.set_input("factor", factor)


class ModelActorScale(SDLExecutorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "actor(model)"

    def get_name(self):
        return "scale"

    def set_factor(self, factor: SDLData):
        self.set_input("factor", factor)


class PhysicalActorTranslate(SDLExecutorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "actor(physical)"

    def get_name(self):
        return "translate"

    def set_factor(self, factor: SDLData):
        self.set_input("factor", factor)


class PhysicalActorRotate(SDLExecutorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "actor(physical)"

    def get_name(self):
        return "rotate"

    def set_axis(self, axis: SDLData):
        self.set_input("axis", axis)

    def set_degree(self, degree: SDLData):
        self.set_input("degree", degree)

    def set_factor(self, factor: SDLData):
        self.set_input("factor", factor)


class PhysicalActorScale(SDLExecutorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "actor(physical)"

    def get_name(self):
        return "scale"

    def set_factor(self, factor: SDLData):
        self.set_input("factor", factor)


class RectangleGeometryCreator(SDLCreatorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "geometry(rectangle)"

    def set_width(self, width: SDLData):
        self.set_input("width", width)

    def set_height(self, height: SDLData):
        self.set_input("height", height)

    def set_texcoord_scale(self, texcoord_scale: SDLData):
        self.set_input("texcoord-scale", texcoord_scale)


class SphereGeometryCreator(SDLCreatorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "geometry(sphere)"

    def set_radius(self, radius: SDLData):
        self.set_input("radius", radius)


class TriangleMeshGeometryCreator(SDLCreatorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "geometry(triangle-mesh)"

    def set_positions(self, positions: SDLData):
        self.set_input("positions", positions)

    def set_texture_coordinates(self, texture_coordinates: SDLData):
        self.set_input("texture-coordinates", texture_coordinates)

    def set_normals(self, normals: SDLData):
        self.set_input("normals", normals)


class ModelLightSourceCreator(SDLCreatorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "light-source(model)"

    def set_geometry(self, geometry: SDLData):
        self.set_input("geometry", geometry)

    def set_material(self, material: SDLData):
        self.set_input("material", material)

    def set_emitted_radiance(self, emitted_radiance: SDLData):
        self.set_input("emitted-radiance", emitted_radiance)

    def set_emit_mode(self, emit_mode: SDLData):
        self.set_input("emit-mode", emit_mode)


class RectangleLightSourceCreator(SDLCreatorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "light-source(rectangle)"

    def set_width(self, width: SDLData):
        self.set_input("width", width)

    def set_height(self, height: SDLData):
        self.set_input("height", height)

    def set_linear_srgb(self, linear_srgb: SDLData):
        self.set_input("linear-srgb", linear_srgb)

    def set_watts(self, watts: SDLData):
        self.set_input("watts", watts)


class SphereLightSourceCreator(SDLCreatorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "light-source(sphere)"

    def set_radius(self, radius: SDLData):
        self.set_input("radius", radius)

    def set_linear_srgb(self, linear_srgb: SDLData):
        self.set_input("linear-srgb", linear_srgb)

    def set_watts(self, watts: SDLData):
        self.set_input("watts", watts)


class AbradedOpaqueMaterialCreator(SDLCreatorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "material(abraded-opaque)"

    def set_type(self, type: SDLData):
        self.set_input("type", type)

    def set_roughness(self, roughness: SDLData):
        self.set_input("roughness", roughness)

    def set_f0(self, f0: SDLData):
        self.set_input("f0", f0)

    def set_roughness_u(self, roughness_u: SDLData):
        self.set_input("roughness-u", roughness_u)

    def set_roughness_v(self, roughness_v: SDLData):
        self.set_input("roughness-v", roughness_v)


class AbradedTranslucentMaterialCreator(SDLCreatorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "material(abraded-translucent)"

    def set_fresnel_type(self, fresnel_type: SDLData):
        self.set_input("fresnel-type", fresnel_type)

    def set_roughness(self, roughness: SDLData):
        self.set_input("roughness", roughness)

    def set_ior_outer(self, ior_outer: SDLData):
        self.set_input("ior-outer", ior_outer)

    def set_ior_inner(self, ior_inner: SDLData):
        self.set_input("ior-inner", ior_inner)


class BinaryMixedSurfaceMaterialCreator(SDLCreatorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "material(binary-mixed-surface)"

    def set_mode(self, mode: SDLData):
        self.set_input("mode", mode)

    def set_factor(self, factor: SDLData):
        self.set_input("factor", factor)

    def set_material_0(self, material_0: SDLData):
        self.set_input("material-0", material_0)

    def set_material_1(self, material_1: SDLData):
        self.set_input("material-1", material_1)


class FullMaterialCreator(SDLCreatorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "material(full)"

    def set_surface(self, surface: SDLData):
        self.set_input("surface", surface)

    def set_interior(self, interior: SDLData):
        self.set_input("interior", interior)

    def set_exterior(self, exterior: SDLData):
        self.set_input("exterior", exterior)


class IdealSubstanceMaterialCreator(SDLCreatorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "material(ideal-substance)"

    def set_type(self, type: SDLData):
        self.set_input("type", type)

    def set_ior_outer(self, ior_outer: SDLData):
        self.set_input("ior-outer", ior_outer)

    def set_ior_inner(self, ior_inner: SDLData):
        self.set_input("ior-inner", ior_inner)

    def set_f0_rgb(self, f0_rgb: SDLData):
        self.set_input("f0-rgb", f0_rgb)

    def set_reflection_scale(self, reflection_scale: SDLData):
        self.set_input("reflection-scale", reflection_scale)

    def set_transmission_scale(self, transmission_scale: SDLData):
        self.set_input("transmission-scale", transmission_scale)


class LayeredSurfaceMaterialCreator(SDLCreatorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "material(layered-surface)"


class LayeredSurfaceMaterialAdd(SDLExecutorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "material(layered-surface)"

    def get_name(self):
        return "add"


class LayeredSurfaceMaterialSet(SDLExecutorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "material(layered-surface)"

    def get_name(self):
        return "set"

    def set_index(self, index: SDLData):
        self.set_input("index", index)

    def set_roughness(self, roughness: SDLData):
        self.set_input("roughness", roughness)

    def set_ior_n(self, ior_n: SDLData):
        self.set_input("ior-n", ior_n)

    def set_ior_k(self, ior_k: SDLData):
        self.set_input("ior-k", ior_k)

    def set_depth(self, depth: SDLData):
        self.set_input("depth", depth)

    def set_g(self, g: SDLData):
        self.set_input("g", g)

    def set_sigma_a(self, sigma_a: SDLData):
        self.set_input("sigma-a", sigma_a)

    def set_sigma_s(self, sigma_s: SDLData):
        self.set_input("sigma-s", sigma_s)


class MatteOpaqueMaterialCreator(SDLCreatorCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "material(matte-opaque)"

    def set_albedo(self, albedo: SDLData):
        self.set_input("albedo", albedo)


class PinholeCameraCreator(SDLCoreCommand):

    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "camera(pinhole)"

    def set_fov_degree(self, fov_degree: SDLData):
        self.set_input("fov-degree", fov_degree)

    def set_film_width_mm(self, film_width_mm: SDLData):
        self.set_input("film-width-mm", film_width_mm)

    def set_film_offset_mm(self, film_offset_mm: SDLData):
        self.set_input("film-offset-mm", film_offset_mm)

    def set_position(self, position: SDLData):
        self.set_input("position", position)

    def set_direction(self, direction: SDLData):
        self.set_input("direction", direction)

    def set_up_axis(self, up_axis: SDLData):
        self.set_input("up-axis", up_axis)


