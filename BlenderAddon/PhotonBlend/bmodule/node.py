from ..utility import settings

import bpy

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


class PhMaterialNodeSocket(bpy.types.NodeSocketShader):

	bl_idname = "PH_MATERIAL_NODE_SOCKET"
	bl_label  = "Photon Socket"

	def __init__(self):
		super().__init__()

	# Blender: draw socket's color
	def draw_color(self, b_context, node):
		return [0.0, 0.0, 0.0, 1.0]


class PhMaterialNode(bpy.types.Node):

	bl_idname = "PH_MATERIAL_NODE"
	bl_label  = "Photon Node"
	bl_icon   = "MATERIAL"

	def draw_buttons(self, b_context, layout):
		pass


class PhMaterialNodeFloatSocket(PhMaterialNodeSocket):

	bl_idname = "PH_MATERIAL_NODE_FLOAT_SOCKET"
	bl_label  = "Photon Float Socket"

	def __init__(self):
		super().__init__()

	# gray
	def draw_color(self, b_context, node):
		return [0.5, 0.5, 0.5, 1.0]




def register():
	bpy.utils.register_class(PhMaterialNodeTree)
	bpy.utils.register_class(PhMaterialNodeFloatSocket)
	bpy.utils.register_class(PhMaterialNode)
	bpy.utils.register_class(CustomPanel)
	bpy.utils.register_class(CustomNode)
	bpy.types.Scene.custom_properties = PointerProperty(type=CustomPropertyGroup)
	nodeitems_utils.register_node_categories("CUSTOM_CATEGORIES", categories)

def unregister():
  bpy.utils.unregister_class(CustomNodeTree)
  bpy.utils.unregister_class(CustomNodeSocket)
  bpy.utils.unregister_class(CustomPropertyGroup)
  bpy.utils.unregister_class(CustomPanel)
  bpy.utils.unregister_class(CustomNode)
  del bpy.types.Scene.custom_properties
nodeitems_utils.unregister_node_categories("CUSTOM_CATEGORIES")


