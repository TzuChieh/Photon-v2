from ..utility import settings
from . import common

import bpy
import nodeitems_utils

from abc import abstractmethod


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

	# Blender: draw socket's color
	def draw_color(self, b_context, node):
		return [0.0, 0.0, 0.0, 1.0]

	# Blender: draw socket
	def draw(self, b_context, b_layout, node, text):
		if self.is_linked or self.is_output:
			b_layout.label(text)
		else:
			row = b_layout.row()
			if hasattr(node.inputs[text], "default_value"):
				row.prop(node.inputs[text], "default_value", text)
			else:
				row.label(text)


class PhMaterialNode(bpy.types.Node):
	bl_idname = "PH_MATERIAL_NODE"
	bl_label  = "Photon Node"
	bl_icon   = "MATERIAL"

	# Blender: called when node created
	def init(self, b_context):
		pass

	# Blender: draw the buttons in node
	def draw_buttons(self, b_context, b_layout):
		pass


class PhSurfaceMaterialSocket(PhMaterialNodeSocket):
	bl_idname = "PH_SURFACE_MATERIAL_SOCKET"
	bl_label  = "Surface Material"

	def draw_color(self, b_context, node):
		return [0.8, 0.1, 0.1, 1.0]  # red


class PhRealSocket(PhMaterialNodeSocket):
	bl_idname = "PH_FLOAT_SOCKET"
	bl_label  = "Real"

	default_value = bpy.props.FloatProperty(name = "Real", default=0.0, min=0.0, max=1.0)

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

	def init(self, b_context):
		self.outputs.new(PhColorSocket.bl_idname, PhColorSocket.bl_label)

	def draw_buttons(self, b_context, b_layout):
		row = b_layout.row()
		row.template_color_picker(self, "color", True)
		row = b_layout.row()
		row.prop(self, "color", "")


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


class PhMaterialNodeCategory(nodeitems_utils.NodeCategory):

	@classmethod
	def poll(cls, b_context):
		return b_context.space_data.tree_type == PhMaterialNodeTree.bl_idname


PH_MATERIAL_NODE_SOCKETS = [
	PhSurfaceMaterialSocket,
	PhRealSocket,
	PhColorSocket
]


PH_MATERIAL_NODES = [
	PhOutputNode,
	PhConstantColorInputNode,
	PhDiffuseSurfaceNode
]


PH_MATERIAL_NODE_CATEGORIES = [
	PhMaterialNodeCategory("OUTPUT", "Output", items = [
		nodeitems_utils.NodeItem(PhOutputNode.bl_idname)
	]),
	PhMaterialNodeCategory("INPUT", "Input", items = [
		nodeitems_utils.NodeItem(PhConstantColorInputNode.bl_idname)
	]),
	PhMaterialNodeCategory("SURFACE_MATERIAL", "Surface Material", items = [
		nodeitems_utils.NodeItem(PhDiffuseSurfaceNode.bl_idname)
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


