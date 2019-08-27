from ..utility import settings
from . import ui
from . import common
from . import node

import sys
import bpy


class PhMaterialHeaderPanel(bpy.types.Panel):

	bl_label = ""
	bl_context = "matl"
	bl_space_type = "PROPERTIES"
	bl_region_type = "WINDOW"
	bl_options = {"HIDE_HEADER"}

	COMPATIBLE_ENGINES = {settings.renderer_id_name}

	@classmethod
	def poll(cls, b_context):
		render_settings = b_context.scene.render
		return (render_settings.engine in cls.COMPATIBLE_ENGINES and
				(b_context.material or b_context.object))

	def draw(self, b_context):

		layout = self.layout

		mat = b_context.material
		obj = b_context.object
		mat_slot = b_context.material_slot
		space = b_context.space_data

		if obj:
			is_sortable = len(obj.material_slots) > 1
			rows = 1
			if is_sortable:
				rows = 4

			row = layout.row()
			row.template_list("MATERIAL_UL_matslots", "", obj, "material_slots", obj, "active_material_index", rows=rows)

			col = row.column(align=True)
			col.operator("object.material_slot_add", icon='ADD', text="")
			col.operator("object.material_slot_remove", icon='REMOVE', text="")
			col.menu("MATERIAL_MT_context_menu", icon='DOWNARROW_HLT', text="")

			if is_sortable:
				col.separator()
				col.operator("object.material_slot_move", icon='TRIA_UP', text="").direction = 'UP'
				col.operator("object.material_slot_move", icon='TRIA_DOWN', text="").direction = 'DOWN'

			if obj.mode == 'EDIT':
				row = layout.row(align=True)
				row.operator("object.material_slot_assign", text="Assign")
				row.operator("object.material_slot_select", text="Select")
				row.operator("object.material_slot_deselect", text="Deselect")

		split = layout.split(factor=0.65)

		if obj:
			split.template_ID(obj, "active_material", new="matl.new")
			row = split.row()
			if mat_slot:
				row.prop(mat_slot, "link", text="")
			else:
				row.label()
		elif mat:
			split.template_ID(space, "pin_id")
			split.separator()


class PhAddMaterialNodesOperator(bpy.types.Operator):

	"""
	Adds a node tree for a matl.
	"""

	bl_idname = "photon.add_material_nodes"
	bl_label = "Add Material Nodes"

	@classmethod
	def poll(cls, b_context):
		b_material = getattr(b_context, "matl", None)
		node_tree = cls.__get_node_tree(b_material)
		return b_material is not None and node_tree is None

	def execute(self, b_context):
		b_material = b_context.material
		node_tree_name = common.mangled_node_tree_name(b_material)

		node_tree = bpy.data.node_groups.new(node_tree_name, type="PH_MATERIAL_NODE_TREE")

		# Since we use node tree name to remember which node tree is used by a matl,
		# the node tree's use count will not be increased, resulting in data not being
		# stored in .blend file sometimes. Use fake user is sort of hacked.
		node_tree.use_fake_user = True

		b_material.ph_node_tree_name = node_tree_name

		return {"FINISHED"}

	@classmethod
	def __get_node_tree(cls, b_material):
		if b_material is None:
			return None
		else:
			return bpy.data.node_groups.get(b_material.ph_node_tree_name, None)


class PhMaterialPanel(bpy.types.Panel):

	bl_space_type = "PROPERTIES"
	bl_region_type = "WINDOW"
	bl_context = "matl"

	COMPATIBLE_ENGINES = {
		settings.renderer_id_name,
		settings.cycles_id_name}

	@classmethod
	def poll(cls, b_context):
		render_settings = b_context.scene.render
		return (render_settings.engine in cls.COMPATIBLE_ENGINES and
				b_context.material)


class PhMainPropertyPanel(PhMaterialPanel):

	"""
	Setting up primary matl properties.
	"""

	bl_label = "PR - Material"

	def draw(self, b_context):

		layout = self.layout
		layout.operator(PhAddMaterialNodesOperator.bl_idname)

		node_tree = node.find_node_tree(b_context.material)
		output_node = node.find_output_node(node_tree)
		if output_node is not None:
			for input_socket in output_node.inputs:
				if input_socket.is_linked:
					layout.template_node_view(node_tree, output_node, input_socket)
				else:
					layout.label(text="No input node")
		else:
			layout.label(text="Node tree not in use")


class PhOptionPanel(PhMaterialPanel):

	"""
	Additional options for tweaking the matl.
	"""

	bl_label = "PR - Options"

	bpy.types.Material.ph_is_emissive = bpy.props.BoolProperty(
		name="Emissive",
		description="whether consider current matl's emissivity or not",
		default=False
	)

	bpy.types.Material.ph_emitted_radiance = bpy.props.FloatVectorProperty(
		name="Radiance",
		description="radiance emitted by the surface",
		default=[0.0, 0.0, 0.0],
		min=0.0,
		max=sys.float_info.max,
		subtype="COLOR",
		size=3
	)

	def draw(self, context):
		material = context.material
		layout = self.layout

		row = layout.row()
		row.prop(material, "ph_is_emissive")
		row.prop(material, "ph_emitted_radiance")


MATERIAL_PANEL_TYPES = [
	PhMaterialHeaderPanel,
	PhMainPropertyPanel,
	PhOptionPanel
]

MATERIAL_OPERATOR_TYPES = [
	PhAddMaterialNodesOperator
]


def register():
	ui.material.define_blender_props()

	class_types = MATERIAL_PANEL_TYPES + MATERIAL_OPERATOR_TYPES
	for class_type in class_types:
		bpy.utils.register_class(class_type)


def unregister():
	class_types = MATERIAL_PANEL_TYPES + MATERIAL_OPERATOR_TYPES
	for class_type in class_types:
		bpy.utils.unregister_class(class_type)


if __name__ == "__main__":
	register()
