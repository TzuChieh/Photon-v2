from ..utility import settings
from . import common
from ..psdl import imagecmd
from ..psdl import materialcmd
from ..psdl import sdlresource
from .. import utility

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

		cmd = materialcmd.FullCreator()
		cmd.set_data_name(res_name)
		cmd.set_surface_ref(surface_mat_res_name)
		sdlconsole.queue_command(cmd)

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
		cmd = imagecmd.ConstantImageCreator()
		cmd.set_data_name(res_name + "_" + self.name + "_" + output_socket.identifier)
		cmd.set_rgb_value(mathutils.Color((self.color[0], self.color[1], self.color[2])))
		if self.usage == "EMISSION":
			cmd.intent_is_emission_srgb()
		elif self.usage == "REFLECTANCE":
			cmd.intent_is_reflectance_srgb()
		sdlconsole.queue_command(cmd)


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
			cmd = imagecmd.ConstantImageCreator()
			albedo_res_name = res_name + "_" + self.name + "_" + albedo_socket.identifier
			cmd.set_data_name(albedo_res_name)
			albedo = albedo_socket.default_value
			cmd.set_rgb_value(mathutils.Color((albedo[0], albedo[1], albedo[2])))
			cmd.intent_is_reflectance_srgb()
			sdlconsole.queue_command(cmd)

		cmd = materialcmd.MatteOpaqueCreator()
		cmd.set_data_name(res_name + "_" + self.name + "_" + surface_material_socket.identifier)
		cmd.set_albedo_image_ref(albedo_res_name)
		sdlconsole.queue_command(cmd)


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

		cmd = materialcmd.BinaryMixedSurfaceCreator()
		cmd.set_data_name(res_name + "_" + self.name + "_" + surface_mat_socket.identifier)
		cmd.set_float_factor(self.factor)
		cmd.set_surface_material0_ref(mat0_res_name)
		cmd.set_surface_material1_ref(mat1_res_name)
		sdlconsole.queue_command(cmd)


class PhAbradedOpaqueNode(PhMaterialNode):
	bl_idname = "PH_ABRADED_OPAQUE"
	bl_label  = "Abraded Opaque"

	roughness = bpy.props.FloatProperty(
		name    = "Roughness",
		default = 0.5,
		min     = 0.0,
		max     = 1.0
	)

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
		self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)

	def draw_buttons(self, b_context, b_layout):
		b_layout.prop(self, "roughness")
		b_layout.prop(self, "f0")

	def to_sdl(self, res_name, sdlconsole):
		surface_mat_socket   = self.outputs[0]
		surface_mat_res_name = res_name + "_" + self.name + "_" + surface_mat_socket.identifier

		cmd = materialcmd.AbradedOpaqueCreator()
		cmd.set_data_name(surface_mat_res_name)
		cmd.set_anisotropicity(False)
		cmd.set_roughness(self.roughness)
		cmd.set_f0(mathutils.Color((self.f0[0], self.f0[1], self.f0[2])))
		sdlconsole.queue_command(cmd)


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
			cmd         = imagecmd.LdrPictureImageCreator()
			image_path  = bpy.path.abspath(self.file_path)
			image_sdlri = sdlresource.SdlResourceIdentifier()
			image_sdlri.append_folder(PhPictureNode.bl_idname + "_pictures")
			image_sdlri.set_file(utility.get_filename(image_path))
			cmd.set_image_sdlri(image_sdlri)

			# copy the file to scene folder
			sdlconsole.create_resource_folder(image_sdlri)
			dst_path = utility.get_appended_path(sdlconsole.get_working_directory(),
			                                     image_sdlri.get_path())
			shutil.copyfile(image_path, dst_path)

		else:
			cmd = imagecmd.ConstantImageCreator()
			cmd.intent_is_raw()
			cmd.set_real_value(0)

		cmd.set_data_name(image_res_name)
		sdlconsole.queue_command(cmd)


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

		cmd = imagecmd.RealMathImageCreator()
		cmd.set_data_name(output_color_res_name)
		cmd.set_operand_image(input_color_res_name)
		cmd.set_multiply()
		cmd.set_real_value(self.factor)
		sdlconsole.queue_command(cmd)


class PhAbradedTranslucentNode(PhMaterialNode):
	bl_idname = "PH_ABRADED_TRANSLUCENT"
	bl_label  = "Abraded Translucent"

	fresnel_type = bpy.props.EnumProperty(
		items=[
			("SCHLICK_APPROX", "Schlick Approx.", ""),
			("EXACT",          "Exact",           "")
		],
		name        = "Fresnel Type",
		description = "Types of Fresnel effect used.",
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

		cmd = materialcmd.AbradedTranslucentCreator()
		cmd.set_data_name(surface_mat_res_name)
		cmd.set_roughness(self.roughness)
		cmd.set_ior_outer(self.ior_outer)
		cmd.set_ior_inner(self.ior_inner)
		if self.fresnel_type == "SCHLICK_APPROX":
			cmd.use_schlick_approx()
		elif self.fresnel_type == "EXACT":
			cmd.use_exact()
		sdlconsole.queue_command(cmd)


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


def to_sdl(res_name, b_material, sdlconsole):

	if b_material is None or b_material.ph_node_tree_name == "":
		print("material <%s> has no node tree, ignoring" % res_name)
		return MaterialNodeTranslateResult()

	node_tree   = bpy.data.node_groups[b_material.ph_node_tree_name]
	output_node = None
	for node in node_tree.nodes:
		if getattr(node, "bl_idname", None) == PhOutputNode.bl_idname:
			output_node = node
			break

	if output_node is None:
		print("material <%s> has no output node, ignoring" % res_name)
		return MaterialNodeTranslateResult()

	processed_nodes = set()
	to_sdl_recursive(res_name, output_node, processed_nodes, sdlconsole)

	return MaterialNodeTranslateResult(output_node.get_surface_emi_res_name(res_name))


PH_MATERIAL_NODE_SOCKETS = [
	PhSurfaceMaterialSocket,
	PhFloatSocket,
	PhColorSocket
]


PH_MATERIAL_NODES = [
	PhOutputNode,
	PhConstantColorInputNode,
	PhDiffuseSurfaceNode,
	PhBinaryMixedSurfaceNode,
	PhAbradedOpaqueNode,
	PhAbradedTranslucentNode,
	PhPictureNode,
	PhMultiplyNode
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
		nodeitems_utils.NodeItem(PhAbradedTranslucentNode.bl_idname)
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


