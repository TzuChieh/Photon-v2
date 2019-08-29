from ..utility import settings
from .material import node, helper
from . import material

import bpy


class PH_MATERIAL_PT_header(bpy.types.Panel):
    bl_label = ""
    bl_context = "material"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_options = {"HIDE_HEADER"}

    COMPATIBLE_ENGINES = {settings.renderer_id_name}

    @classmethod
    def poll(cls, b_context):
        render_settings = b_context.scene.render
        return (
            render_settings.engine in cls.COMPATIBLE_ENGINES and
            (b_context.material or b_context.object)
        )

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
            split.template_ID(obj, "active_material", new="material.new")
            row = split.row()
            if mat_slot:
                row.prop(mat_slot, "link", text="")
            else:
                row.label()
        elif mat:
            split.template_ID(space, "pin_id")
            split.separator()


class PH_MATERIAL_OT_add_material_nodes(bpy.types.Operator):
    """
    Adds a node tree for a material.
    """

    bl_idname = "photon.add_material_nodes"
    bl_label = "Add Material Nodes"

    @classmethod
    def poll(cls, b_context):
        b_material = getattr(b_context, "material", None)
        node_tree = b_material.photon.node_tree
        return b_material is not None and node_tree is None

    def execute(self, b_context):
        b_material = b_context.material

        node_tree = bpy.data.node_groups.new(b_material.name, type="PH_MATERIAL_NODE_TREE")
        b_material.photon.node_tree = node_tree
        b_material.photon.use_nodes = True

        # Since we use node tree name to remember which node tree is used by a material,
        # the node tree's use count will not be increased, resulting in data not being
        # stored in .blend file sometimes. Use fake user is sort of hacked.
        # node_tree.use_fake_user = True

        return {"FINISHED"}


class PhMaterialPanel(bpy.types.Panel):
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "material"

    COMPATIBLE_ENGINES = {
        settings.renderer_id_name,
        settings.cycles_id_name
    }

    @classmethod
    def poll(cls, b_context):
        render_settings = b_context.scene.render
        return (
            render_settings.engine in cls.COMPATIBLE_ENGINES and
            b_context.material
        )


class PH_MATERIAL_PT_properties(PhMaterialPanel):
    """
    Setting up primary material properties.
    """

    bl_label = "Main Properties"

    def draw(self, b_context):

        b_material = b_context.material

        layout = self.layout
        layout.operator(PH_MATERIAL_OT_add_material_nodes.bl_idname)

        layout.prop(b_material, "use_nodes", text="Use Shader Nodes")

        # Show traditional UI for shader nodes.
        node_tree = helper.find_node_tree(b_context.material)
        output_node = helper.find_output_node(node_tree)
        if output_node is not None:
            for input_socket in output_node.inputs:
                if input_socket.is_linked:
                    layout.template_node_view(node_tree, output_node, input_socket)
                else:
                    layout.label(text="No input node")
        else:
            layout.label(text="Material node not in use")


# class PhOptionPanel(PhMaterialPanel):
#
#     """
#     Additional options for tweaking the material.
#     """
#
#     bl_label = "Options"
#
#     bpy.types.Material.ph_is_emissive = bpy.props.BoolProperty(
#         name="Emissive",
#         description="whether consider current material's emissivity or not",
#         default=False
#     )
#
#     bpy.types.Material.ph_emitted_radiance = bpy.props.FloatVectorProperty(
#         name="Radiance",
#         description="radiance emitted by the surface",
#         default=[0.0, 0.0, 0.0],
#         min=0.0,
#         max=sys.float_info.max,
#         subtype="COLOR",
#         size=3
#     )
#
#     def draw(self, context):
#         material = context.material
#         layout = self.layout
#
#         row = layout.row()
#         row.prop(material, "ph_is_emissive")
#         row.prop(material, "ph_emitted_radiance")


MATERIAL_PANELS = [
    PH_MATERIAL_PT_header,
    PH_MATERIAL_PT_properties,
    # PhOptionPanel
]

MATERIAL_OPERATORS = [
    PH_MATERIAL_OT_add_material_nodes
]


def include_module(module_manager):
    classes = MATERIAL_PANELS + MATERIAL_OPERATORS
    for clazz in classes:
        module_manager.add_class(clazz)

    material.include_module(module_manager)
