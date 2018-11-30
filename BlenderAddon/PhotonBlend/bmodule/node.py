from ..utility import settings
from . import common
from ..psdl import imagecmd
from ..psdl import materialcmd
from ..psdl import sdlresource
from .. import utility
from ..psdl import clause
from ..psdl.cmd import RawCommand

from ..psdl.pysdl import (
	SDLInteger,
	SDLReal,
	SDLString,
	SDLVector3,
	SDLReference)

from ..psdl.pysdl import (
	MatteOpaqueMaterialCreator,
	AbradedOpaqueMaterialCreator,
	AbradedTranslucentMaterialCreator,
	LayeredSurfaceMaterialCreator,
	LayeredSurfaceMaterialAdd,
	LayeredSurfaceMaterialSet,
	FullMaterialCreator,
	BinaryMixedSurfaceMaterialCreator,
	ConstantImageCreator,
	LdrPictureImageCreator,
	RealMathImageCreator,
	IdealSubstanceMaterialCreator)

import bpy
import nodeitems_utils
import mathutils

import shutil
import sys
from abc import abstractmethod


class MaterialNodeTranslateResult:
	def __init__(self, surface_emi_res_name = None):
		self.surface_emi_res_name = surface_emi_res_name

	def is_surface_emissive(self):
		return self.surface_emi_res_name is not None


class PhMaterialNodeTree(bpy.types.NodeTree):

	bl_idname = "PH_MATERIAL_NODE_TREE"
	bl_label  = "Photon Node Tree"
	bl_icon   = "MATERIAL"

	COMPATIBLE_ENGINES = {settings.renderer_id_name}

	@classmethod
	def poll(cls, b_context):
		render_settings = b_context.scene.render
		return render_settings.engine in cls.COMPATIBLE_ENGINES

	# Blender: set the current node tree to the one the active material owns (update editor views)
	@classmethod
	def get_from_context(cls, b_context):
		obj = b_context.active_object
		if obj and obj.type not in {"LAMP", "CAMERA"}:
			mat = obj.active_material
			if mat is not None:
				node_tree_name = mat.ph_node_tree_name
				if node_tree_name != "":
					return bpy.data.node_groups[node_tree_name], mat, mat
		return None, None, None


class PhMaterialNodeHeader(bpy.types.Header):
	bl_space_type = "NODE_EDITOR"

	def draw(self, b_context):
		b_layout = self.layout
		obj      = b_context.object

		# TODO: remove node tree selection menu and prepend material.new like cycles

		if obj and obj.type not in {"LAMP", "CAMERA"}:
			row = b_layout.row()

			# Show material.new when no active material exists
			row.template_ID(obj, "active_material", new = "material.new")


class PhMaterialNodeSocket(bpy.types.NodeSocketShader):

	bl_idname = "PH_MATERIAL_NODE_SOCKET"
	bl_label  = "Photon Socket"

	link_only = bpy.props.BoolProperty(
		name        = "Link Only",
		description = "Makes this node for linking only, its contained value(s) is ignored.",
		default     = False
	)

	# Blender: draw socket's color
	def draw_color(self, b_context, node):
		return [0.0, 0.0, 0.0, 1.0]

	# Blender: draw socket
	def draw(self, b_context, b_layout, node, text):
		if node.bl_idname != PhOutputNode.bl_idname:
			if self.is_linked or self.is_output:
				b_layout.label(text)
			else:
				if hasattr(self, "default_value"):
					b_layout.prop(self, "default_value", text)
				else:
					b_layout.label(text)
		else:
			b_layout.label(text)

	def get_from_res_name(self, res_name, link_index = 0):
		if not self.links:
			return None
		from_node   = self.links[link_index].from_node
		from_socket = self.links[link_index].from_socket
		return res_name + "_" + from_node.name + "_" + from_socket.identifier


class PhMaterialNode(bpy.types.Node):
	bl_idname = "PH_MATERIAL_NODE"
	bl_label  = "Photon Node"
	bl_icon   = "MATERIAL"

	@classmethod
	def poll(cls, b_node_tree):
		return b_node_tree.bl_idname == PhMaterialNodeTree.bl_idname

	# Blender: called when node created
	def init(self, b_context):
		pass

	# Blender: draw properties in node
	def draw_buttons(self, b_context, b_layout):
		pass

	@abstractmethod
	def to_sdl(self, res_name, sdlconsole):
		pass


class PhSurfaceMaterialSocket(PhMaterialNodeSocket):
	bl_idname = "PH_SURFACE_MATERIAL_SOCKET"
	bl_label  = "Surface Material"

	def draw_color(self, b_context, node):
		return [0.8, 0.1, 0.1, 1.0]  # red


class PhFloatSocket(PhMaterialNodeSocket):
	bl_idname = "PH_FLOAT_SOCKET"
	bl_label  = "Real"

	default_value = bpy.props.FloatProperty(
		name    = "Float",
		default = 0.5,
		min     = 0.0,
		max     = 1.0
	)

	def draw_color(self, b_context, node):
		return [0.5, 0.5, 0.5, 1.0]  # gray


class PhColorSocket(PhMaterialNodeSocket):
	bl_idname = "PH_COLOR_SOCKET"
	bl_label  = "Color"

	default_value = bpy.props.FloatVectorProperty(
		name        = "Color",
		description = "color value",
		default     = [0.5, 0.5, 0.5],
		min         = 0.0,
		max         = 1.0,
		subtype     = "COLOR",
		size        = 3
	)

	def draw_color(self, b_context, node):
		return [0.7, 0.7, 0.1, 1.0]  # yellow


class PhSurfaceLayerSocket(PhMaterialNodeSocket):
	bl_idname = "PH_SURFACE_LAYER_SOCKET"
	bl_label  = "Surface Layer"

	def draw_color(self, b_context, node):
		return [0.0, 0.0, 0.0, 1.0]  # black


class PhOutputNode(PhMaterialNode):
	bl_idname = "PH_OUTPUT"
	bl_label  = "Output"

	def init(self, b_context):
		self.inputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)
		self.inputs.new(PhColorSocket.bl_idname, "Surface Emission")
		self.inputs[1].link_only = True

	def to_sdl(self, res_name, sdlconsole):
		surface_mat_socket   = self.inputs[0]
		surface_mat_res_name = surface_mat_socket.get_from_res_name(res_name)
		if surface_mat_res_name is None:
			print("material <%s>'s output node is not linked, ignored" % res_name)
			return

		creator = FullMaterialCreator()
		creator.set_data_name(res_name)
		creator.set_surface(SDLReference("material", surface_mat_res_name))
		sdlconsole.queue_command(creator)

	def get_surface_emi_res_name(self, res_name):
		surface_emi_socket = self.inputs[1]
		return surface_emi_socket.get_from_res_name(res_name)


class PhConstantColorInputNode(PhMaterialNode):
	bl_idname = "PH_CONSTANT_COLOR"
	bl_label  = "Constant Color"

	color = bpy.props.FloatVectorProperty(
		name        = "Color",
		description = "color value",
		default     = [0.5, 0.5, 0.5],
		min         = 0.0,
		max         = 1.0,
		subtype     = "COLOR",
		size        = 3
	)

	usage = bpy.props.EnumProperty(
		items = [
			("EMISSION",    "Emission",    ""),
			("REFLECTANCE", "Reflectance", "")
		],
		name        = "Usage",
		description = "What is this color for?",
		default     = "REFLECTANCE"
	)

	def init(self, b_context):
		self.outputs.new(PhColorSocket.bl_idname, PhColorSocket.bl_label)

	def draw_buttons(self, b_context, b_layout):
		b_layout.template_color_picker(self, "color", True)
		b_layout.prop(self, "color", "")
		b_layout.prop(self, "usage", "")

	def to_sdl(self, res_name, sdlconsole):
		output_socket = self.outputs[0]
		creator = ConstantImageCreator()
		creator.set_data_name(res_name + "_" + self.name + "_" + output_socket.identifier)
		creator.set_value(SDLVector3(mathutils.Color((self.color[0], self.color[1], self.color[2]))))
		if self.usage == "EMISSION":
			creator.set_value_type(SDLString("emr-linear-srgb"))
		elif self.usage == "REFLECTANCE":
			creator.set_value_type(SDLString("ecf-linear-srgb"))
		sdlconsole.queue_command(creator)


class PhDiffuseSurfaceNode(PhMaterialNode):
	bl_idname = "PH_DIFFUSE_SURFACE"
	bl_label  = "Diffuse Surface"

	diffusion_type = bpy.props.EnumProperty(
		items = [
			("LAMBERTIAN", "Lambertian", "")
		],
		name        = "Type",
		description = "surface diffusion types",
		default     = "LAMBERTIAN"
	)

	def init(self, b_context):
		self.inputs.new(PhColorSocket.bl_idname, "Albedo")
		self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)

	def draw_buttons(self, b_context, b_layout):
		row = b_layout.row()
		row.prop(self, "diffusion_type", "")

	def to_sdl(self, res_name, sdlconsole):
		albedo_socket           = self.inputs[0]
		surface_material_socket = self.outputs[0]

		albedo_res_name = albedo_socket.get_from_res_name(res_name)
		if albedo_res_name is None:
			creator = ConstantImageCreator()
			albedo_res_name = res_name + "_" + self.name + "_" + albedo_socket.identifier
			creator.set_data_name(albedo_res_name)
			albedo = albedo_socket.default_value
			creator.set_value(SDLVector3(mathutils.Color((albedo[0], albedo[1], albedo[2]))))
			creator.set_value_type(SDLString("ecf-linear-srgb"))
			sdlconsole.queue_command(creator)

		creator = MatteOpaqueMaterialCreator()
		creator.set_data_name(res_name + "_" + self.name + "_" + surface_material_socket.identifier)
		creator.set_albedo(SDLReference("image", albedo_res_name))
		sdlconsole.queue_command(creator)


class PhBinaryMixedSurfaceNode(PhMaterialNode):
	bl_idname = "PH_BINARY_MIXED_SURFACE"
	bl_label  = "Binary Mixed Surface"

	factor = bpy.props.FloatProperty(
		name    = "Factor",
		default = 0.5,
		min     = 0.0,
		max     = 1.0
	)

	def init(self, b_context):
		self.inputs.new(PhSurfaceMaterialSocket.bl_idname, "Material A")
		self.inputs.new(PhSurfaceMaterialSocket.bl_idname, "Material B")
		self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)

	def draw_buttons(self, b_context, b_layout):
		row = b_layout.row()
		row.prop(self, "factor")

	def to_sdl(self, res_name, sdlconsole):
		mat0_socket        = self.inputs[0]
		mat1_socket        = self.inputs[1]
		surface_mat_socket = self.outputs[0]

		mat0_res_name = mat0_socket.get_from_res_name(res_name)
		mat1_res_name = mat1_socket.get_from_res_name(res_name)
		if mat0_res_name is None or mat1_res_name is None:
			print("warning: material <%s>'s binary mixed surface node is incomplete" % res_name)
			return

		creator = BinaryMixedSurfaceMaterialCreator()
		creator.set_data_name(res_name + "_" + self.name + "_" + surface_mat_socket.identifier)
		creator.set_factor(SDLReal(self.factor))
		creator.set_material_0(SDLReference("material", mat0_res_name))
		creator.set_material_1(SDLReference("material", mat1_res_name))
		sdlconsole.queue_command(creator)


class PhAbradedOpaqueNode(PhMaterialNode):
	bl_idname = "PH_ABRADED_OPAQUE"
	bl_label  = "Abraded Opaque"

	f0 = bpy.props.FloatVectorProperty(
		name        = "Color",
		description = "color value",
		default     = [0.5, 0.5, 0.5],
		min         = 0.0,
		max         = 1.0,
		subtype     = "COLOR",
		size        = 3
	)

	def init(self, b_context):
		self.inputs.new(PhFloatSocket.bl_idname, "Roughness")
		self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)

	def draw_buttons(self, b_context, b_layout):
		b_layout.prop(self, "f0")

	def to_sdl(self, res_name, sdlconsole):

		surface_mat_socket   = self.outputs[0]
		surface_mat_res_name = res_name + "_" + self.name + "_" + surface_mat_socket.identifier

		creator = AbradedOpaqueMaterialCreator()
		creator.set_data_name(surface_mat_res_name)
		creator.set_type(SDLString("iso-metallic-ggx"))
		creator.set_roughness(SDLReal(self.inputs[0].default_value))
		creator.set_f0(SDLVector3(mathutils.Color((self.f0[0], self.f0[1], self.f0[2]))))
		sdlconsole.queue_command(creator)


class PhPictureNode(bpy.types.Node):
	bl_idname = "PH_PICTURE"
	bl_label  = "Picture"

	file_path = bpy.props.StringProperty(
		name    = "File",
		default = "",
		subtype = "FILE_PATH"
	)

	def init(self, b_context):
		self.outputs.new(PhColorSocket.bl_idname, PhColorSocket.bl_label)

	def draw_buttons(self, b_context, b_layout):
		b_layout.prop(self, "file_path")

	def to_sdl(self, res_name, sdlconsole):

		image_socket   = self.outputs[0]
		image_res_name = res_name + "_" + self.name + "_" + image_socket.identifier

		if self.file_path != "":

			creator = LdrPictureImageCreator()
			image_path  = bpy.path.abspath(self.file_path)
			image_sdlri = sdlresource.SdlResourceIdentifier()
			image_sdlri.append_folder(PhPictureNode.bl_idname + "_pictures")
			image_sdlri.set_file(utility.get_filename(image_path))
			creator.set_image(SDLString(image_sdlri.get_identifier()))

			# copy the file to scene folder
			sdlconsole.create_resource_folder(image_sdlri)
			dst_path = utility.get_appended_path(sdlconsole.get_working_directory(),
			                                     image_sdlri.get_path())
			shutil.copyfile(image_path, dst_path)

		else:

			creator = ConstantImageCreator()
			creator.set_value_type(SDLString("raw"))
			creator.set_value(SDLReal(0))

		creator.set_data_name(image_res_name)
		sdlconsole.queue_command(creator)


class PhMultiplyNode(PhMaterialNode):
	bl_idname = "PH_MULTIPLY"
	bl_label  = "Multiply"

	factor = bpy.props.FloatProperty(
		name    = "Factor",
		default = 1.0,
		min     = sys.float_info.min,
		max     = sys.float_info.max
	)

	def init(self, b_context):
		self.inputs.new(PhColorSocket.bl_idname, PhColorSocket.bl_label)
		self.outputs.new(PhColorSocket.bl_idname, PhColorSocket.bl_label)

	def draw_buttons(self, b_context, b_layout):
		b_layout.prop(self, "factor")

	def to_sdl(self, res_name, sdlconsole):
		input_color_socket    = self.inputs[0]
		output_color_socket   = self.outputs[0]
		input_color_res_name  = input_color_socket.get_from_res_name(res_name)
		output_color_res_name = res_name + "_" + self.name + "_" + output_color_socket.identifier
		if input_color_res_name is None:
			print("warning: node <%s> has no input linked, ignoring" % self.name)
			return

		creator = RealMathImageCreator()
		creator.set_data_name(SDLString(output_color_res_name))
		creator.set_operand(SDLReference("image", input_color_res_name))
		creator.set_math_op(SDLString("multiply"))
		creator.set_value(SDLReal(self.factor))
		sdlconsole.queue_command(creator)


class PhAbradedTranslucentNode(PhMaterialNode):
	bl_idname = "PH_ABRADED_TRANSLUCENT"
	bl_label  = "Abraded Translucent"

	fresnel_type = bpy.props.EnumProperty(
		items=[
			("SCHLICK_APPROX", "Schlick Approx.", ""),
			("EXACT",          "Exact",           "")
		],
		name        = "Fresnel Type",
		description = "Type of Fresnel effect used.",
		default     = "EXACT"
	)

	roughness = bpy.props.FloatProperty(
		name    = "Roughness",
		default = 0.5,
		min     = 0.0,
		max     = 1.0
	)

	ior_outer = bpy.props.FloatProperty(
		name    = "IOR Outer",
		default = 1.0,
		min     = 0.0,
		max     = sys.float_info.max
	)

	ior_inner = bpy.props.FloatProperty(
		name    = "IOR Inner",
		default = 1.5,
		min     = 0.0,
		max     = sys.float_info.max
	)

	def init(self, b_context):
		self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)

	def draw_buttons(self, b_context, b_layout):
		b_layout.prop(self, "fresnel_type", "")
		b_layout.prop(self, "roughness")
		b_layout.prop(self, "ior_outer")
		b_layout.prop(self, "ior_inner")

	def to_sdl(self, res_name, sdlconsole):
		surface_mat_socket   = self.outputs[0]
		surface_mat_res_name = res_name + "_" + self.name + "_" + surface_mat_socket.identifier

		creator = AbradedTranslucentMaterialCreator()
		creator.set_data_name(surface_mat_res_name)
		creator.set_roughness(SDLReal(self.roughness))
		creator.set_ior_inner(SDLReal(self.ior_inner))
		creator.set_ior_outer(SDLReal(self.ior_outer))
		if self.fresnel_type == "SCHLICK_APPROX":
			creator.set_fresnel_type(SDLString("schlick-approx"))
		elif self.fresnel_type == "EXACT":
			creator.set_fresnel_type(SDLString("exact"))
		sdlconsole.queue_command(creator)


class PhSurfaceLayerNode(PhMaterialNode):
	bl_idname = "PH_SURFACE_LAYER"
	bl_label  = "Surface Layer"

	roughness = bpy.props.FloatProperty(
		name    = "Roughness",
		default = 0.5,
		min     = 0.0,
		max     = 1.0
	)

	ior_type = bpy.props.EnumProperty(
		items = [
			("SCALAR", "Scalar", ""),
			("RGB",    "RGB",    "")
		],
		name        = "IoR Type",
		description = "Type of IoR data used.",
		default     = "SCALAR"
	)

	ior_n = bpy.props.FloatProperty(
		name    = "IoR N",
		default = 1.5,
		min     = 0.0,
		max     = sys.float_info.max
	)

	ior_k = bpy.props.FloatProperty(
		name    = "IoR K",
		default = 0.0,
		min     = 0.0,
		max     = sys.float_info.max
	)

	ior_n_rgb = bpy.props.FloatVectorProperty(
		name        = "IoR N",
		description = "RGB value of IoR N.",
		default     = [1.5, 1.5, 1.5],
		min         = 0.0,
		max         = sys.float_info.max,
		subtype     = "COLOR",
		size        = 3
	)

	ior_k_rgb = bpy.props.FloatVectorProperty(
		name        = "IoR K",
		description = "RGB value of IoR K.",
		default     = [0.0, 0.0, 0.0],
		min         = 0.0,
		max         = sys.float_info.max,
		subtype     = "COLOR",
		size        = 3
	)

	depth = bpy.props.FloatProperty(
		name    = "depth",
		default = 0.0,
		min     = 0.0,
		max     = sys.float_info.max
	)

	g = bpy.props.FloatProperty(
		name    = "g",
		default = 0.9,
		min     = 0.5,
		max     = 1.0
	)

	sigma_a = bpy.props.FloatProperty(
		name    ="Sigma A",
		default = 0.1,
		min     = 0.0,
		max     = sys.float_info.max
	)

	sigma_s = bpy.props.FloatProperty(
		name    = "Sigma S",
		default = 0.1,
		min     = 0.0,
		max     = sys.float_info.max
	)

	def init(self, b_context):
		self.outputs.new(PhSurfaceLayerSocket.bl_idname, PhSurfaceLayerSocket.bl_label)

	def draw_buttons(self, b_context, b_layout):
		b_layout.prop(self, "ior_type")
		b_layout.prop(self, "roughness")

		if self.ior_type == "SCALAR":
			b_layout.prop(self, "ior_n")
			b_layout.prop(self, "ior_k")
		elif self.ior_type == "RGB":
			b_layout.prop(self, "ior_n_rgb")
			b_layout.prop(self, "ior_k_rgb")

		b_layout.prop(self, "depth")
		b_layout.prop(self, "g")
		b_layout.prop(self, "sigma_a")
		b_layout.prop(self, "sigma_s")

	def to_sdl(self, res_name, sdlconsole):
		pass


class PhLayeredSurfaceNode(PhMaterialNode):
	bl_idname = "PH_LAYERED_SURFACE"
	bl_label  = "Layered Surface"

	def update_inputs(self, b_context):

		specified_num_layers = self["num_layers"]

		while len(self.inputs) != specified_num_layers:
			if len(self.inputs) < specified_num_layers:
				self.inputs.new(PhSurfaceLayerSocket.bl_idname, PhSurfaceLayerSocket.bl_label)
			else:
				self.inputs.remove(self.inputs[len(self.inputs) - 1])

	num_layers = bpy.props.IntProperty(
		name    = "# Layers",
		default = 1,
		min     = 1,
		max     = 1024,
		update  = update_inputs
	)

	def init(self, b_context):
		self.inputs.new(PhSurfaceLayerSocket.bl_idname, PhSurfaceLayerSocket.bl_label)
		self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)

	def draw_buttons(self, b_context, b_layout):
		b_layout.prop(self, "num_layers")

	def to_sdl(self, res_name, sdlconsole):
		surface_mat_socket   = self.outputs[0]
		surface_mat_res_name = res_name + "_" + self.name + "_" + surface_mat_socket.identifier

		creator = LayeredSurfaceMaterialCreator()
		creator.set_data_name(surface_mat_res_name)
		sdlconsole.queue_command(creator)

		for i in range(0, len(self.inputs)):
			if not self.inputs[i].links:
				continue

			layer_node = self.inputs[i].links[0].from_node

			adder = LayeredSurfaceMaterialAdd()
			adder.set_target_name(surface_mat_res_name)
			sdlconsole.queue_command(adder)

			setter = LayeredSurfaceMaterialSet()
			setter.set_target_name(surface_mat_res_name)
			setter.set_index(SDLInteger(i))

			setter.set_roughness(SDLReal(layer_node.roughness))
			if layer_node.ior_type == "SCALAR":
				setter.set_ior_n(SDLReal(layer_node.ior_n))
				setter.set_ior_k(SDLReal(layer_node.ior_k))
			elif layer_node.ior_type == "RGB":
				setter.set_ior_n(SDLVector3(layer_node.ior_n))
				setter.set_ior_k(SDLVector3(layer_node.ior_k))
			setter.set_depth(SDLReal(layer_node.depth))
			setter.set_g(SDLReal(layer_node.g))
			setter.set_sigma_a(SDLReal(layer_node.sigma_a))
			setter.set_sigma_s(SDLReal(layer_node.sigma_s))

			sdlconsole.queue_command(setter)


class PhIdealSubstanceNode(PhMaterialNode):
	bl_idname = "PH_IDEAL_SUBSTANCE"
	bl_label  = "Ideal Substance"

	substance_type = bpy.props.EnumProperty(
		items = [
			("DIELECTRIC_REFLECTOR",   "Dielectric Reflector",   ""),
			("METALLIC_REFLECTOR",     "Metallic Reflector",     ""),
			("DIELECTRIC_TRANSMITTER", "Dielectric Transmitter", ""),
			("DIELECTRIC",             "Dielectric",             "")
		],
		name        = "Substance Type",
		description = "Type of ideal substancee.",
		default     = "METALLIC_REFLECTOR"
	)

	f0 = bpy.props.FloatVectorProperty(
		name        = "F0",
		description = "F0 value",
		default     = [0.9, 0.9, 0.9],
		min         = 0.0,
		max         = 1.0,
		subtype     = "COLOR",
		size        = 3
	)

	ior_outer = bpy.props.FloatProperty(
		name    = "IOR Outer",
		default = 1.0,
		min     = .0,
		max     = sys.float_info.max
	)

	ior_inner = bpy.props.FloatProperty(
		name    = "IOR Inner",
		default = 1.5,
		min     = 0.0,
		max     = sys.float_info.max
	)

	reflection_scale = bpy.props.FloatVectorProperty(
		name="Reflection Scale",
		description="for artistic control",
		default=[1.0, 1.0, 1.0],
		min=0.0,
		max=sys.float_info.max,
		subtype="COLOR",
		size=3
	)

	transmission_scale = bpy.props.FloatVectorProperty(
		name="Transmission Scale",
		description="for artistic control",
		default=[1.0, 1.0, 1.0],
		min=0.0,
		max=1.0,
		subtype="COLOR",
		size=3
	)

	def init(self, b_context):
		self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)

	def draw_buttons(self, b_context, b_layout):
		b_layout.prop(self, "substance_type", "")
		b_layout.prop(self, "ior_outer")

		if (self.substance_type == "DIELECTRIC_REFLECTOR" or
		    self.substance_type == "DIELECTRIC_TRANSMITTER" or
		    self.substance_type == "DIELECTRIC"):
			b_layout.prop(self, "ior_inner")

		if self.substance_type == "METALLIC_REFLECTOR":
			b_layout.prop(self, "f0")

		b_layout.prop(self, "reflection_scale")
		b_layout.prop(self, "transmission_scale")

	def to_sdl(self, res_name, sdlconsole):

		surface_mat_socket   = self.outputs[0]
		surface_mat_res_name = res_name + "_" + self.name + "_" + surface_mat_socket.identifier

		creator = IdealSubstanceMaterialCreator()
		creator.set_data_name(surface_mat_res_name)
		creator.set_ior_outer(SDLReal(self.ior_outer))
		creator.set_ior_inner(SDLReal(self.ior_inner))
		creator.set_f0_rgb(SDLVector3(self.f0))
		creator.set_reflection_scale(SDLVector3(self.reflection_scale))
		creator.set_transmission_scale(SDLVector3(self.transmission_scale))

		if self.substance_type == "DIELECTRIC_REFLECTOR":
			creator.set_type(SDLString("dielectric-reflector"))
		elif self.substance_type == "METALLIC_REFLECTOR":
			creator.set_type(SDLString("metallic-reflector"))
		elif self.substance_type == "DIELECTRIC_TRANSMITTER":
			creator.set_type(SDLString("transmitter"))
		elif self.substance_type == "DIELECTRIC":
			creator.set_type(SDLString("dielectric"))

		sdlconsole.queue_command(creator)


class PhMaterialNodeCategory(nodeitems_utils.NodeCategory):

	@classmethod
	def poll(cls, b_context):
		return b_context.space_data.tree_type == PhMaterialNodeTree.bl_idname


def to_sdl_recursive(res_name, current_node, processed_nodes, sdlconsole):

	for socket in current_node.inputs:
		for link in socket.links:
			from_node = link.from_node
			if from_node not in processed_nodes:
				to_sdl_recursive(res_name, from_node, processed_nodes, sdlconsole)
				processed_nodes.add(from_node)

	current_node.to_sdl(res_name, sdlconsole)


def find_node_tree(b_material):

	if b_material is None or b_material.ph_node_tree_name == "":
		return None

	return bpy.data.node_groups[b_material.ph_node_tree_name]


def find_output_node(node_tree):

	if node_tree is None:
		return None

	for node in node_tree.nodes:
		if getattr(node, "bl_idname", None) == PhOutputNode.bl_idname:
			return node

	return None


def to_sdl(res_name, b_material, sdlconsole):

	node_tree = find_node_tree(b_material)
	output_node = find_output_node(node_tree)
	if output_node is None:
		print("material <%s> has no output node, ignoring" % res_name)
		return MaterialNodeTranslateResult()

	processed_nodes = set()
	to_sdl_recursive(res_name, output_node, processed_nodes, sdlconsole)

	return MaterialNodeTranslateResult(output_node.get_surface_emi_res_name(res_name))


PH_MATERIAL_NODE_SOCKETS = [
	PhSurfaceMaterialSocket,
	PhFloatSocket,
	PhColorSocket,
	PhSurfaceLayerSocket
]


PH_MATERIAL_NODES = [
	PhOutputNode,
	PhConstantColorInputNode,
	PhDiffuseSurfaceNode,
	PhBinaryMixedSurfaceNode,
	PhAbradedOpaqueNode,
	PhAbradedTranslucentNode,
	PhPictureNode,
	PhMultiplyNode,
	PhLayeredSurfaceNode,
	PhSurfaceLayerNode,
	PhIdealSubstanceNode
]


PH_MATERIAL_NODE_CATEGORIES = [
	PhMaterialNodeCategory("OUTPUT", "Output", items = [
		nodeitems_utils.NodeItem(PhOutputNode.bl_idname)
	]),
	PhMaterialNodeCategory("INPUT", "Input", items = [
		nodeitems_utils.NodeItem(PhConstantColorInputNode.bl_idname),
		nodeitems_utils.NodeItem(PhPictureNode.bl_idname)
	]),
	PhMaterialNodeCategory("SURFACE_MATERIAL", "Surface Material", items = [
		nodeitems_utils.NodeItem(PhDiffuseSurfaceNode.bl_idname),
		nodeitems_utils.NodeItem(PhBinaryMixedSurfaceNode.bl_idname),
		nodeitems_utils.NodeItem(PhAbradedOpaqueNode.bl_idname),
		nodeitems_utils.NodeItem(PhAbradedTranslucentNode.bl_idname),
		nodeitems_utils.NodeItem(PhLayeredSurfaceNode.bl_idname),
		nodeitems_utils.NodeItem(PhSurfaceLayerNode.bl_idname),
		nodeitems_utils.NodeItem(PhIdealSubstanceNode.bl_idname)
	]),
	PhMaterialNodeCategory("MATH", "Math", items = [
		nodeitems_utils.NodeItem(PhMultiplyNode.bl_idname)
	])
]


def register():

	bpy.utils.register_class(PhMaterialNodeTree)

	for socket_type in PH_MATERIAL_NODE_SOCKETS:
		bpy.utils.register_class(socket_type)

	for node_type in PH_MATERIAL_NODES:
		bpy.utils.register_class(node_type)

	bpy.utils.register_class(PhMaterialNodeHeader)
	nodeitems_utils.register_node_categories("PH_MATERIAL_NODE_CATEGORIES", PH_MATERIAL_NODE_CATEGORIES)


def unregister():

	bpy.utils.unregister_class(PhMaterialNodeTree)

	for socket_type in PH_MATERIAL_NODE_SOCKETS:
		bpy.utils.unregister_class(socket_type)

	for node_type in PH_MATERIAL_NODES:
		bpy.utils.unregister_class(node_type)

	bpy.utils.unregister_node_categories(PhMaterialNodeHeader)
	nodeitems_utils.unregister_node_categories("PH_MATERIAL_NODE_CATEGORIES")


