from ..node_base import (
        PhSurfaceMaterialNode,
        PhFloatFactorSocket,
        PhColorSocket,
        PhSurfaceMaterialSocket)
from psdl import sdl

import bpy


class PhBinaryMixedSurfaceNode(PhSurfaceMaterialNode):
    bl_idname = 'PH_BINARY_MIXED_SURFACE'
    bl_label = "Binary Mixed Surface"

    # DEFAULT_NODE_NAME = "__" + bl_idname + "_default_node"

    factor_type: bpy.props.EnumProperty(
        items=[
            ('FLOAT', "Float Factor", "Determine mix factor of all color channels together", 0),
            ('COLOR', "Color Factor", "Determine mix factor of each color channel", 1)
        ],
        name="Factor Type",
        description="Type of mixing factor",
        default='FLOAT'
    )

    def to_sdl(self, b_material, sdlconsole):
        mat0_res_name = self.get_linked_input_resource_name(b_material, 0)
        mat1_res_name = self.get_linked_input_resource_name(b_material, 1)
        # TODO: use the default_value defined albedo
        if mat0_res_name is None or mat1_res_name is None:
            self.warn_incomplete_node(b_material, "material A or material B input is not linked")
            self.queue_fallback_material(sdlconsole, self.get_output_resource_name(b_material))
            return

        factor_input_index = 2 if self.factor_type == 'FLOAT' else 3
        factor_res_name = self.get_linked_input_resource_name(b_material, factor_input_index)

        creator = sdl.BinaryMixedSurfaceMaterialCreator()
        creator.set_data_name(self.get_output_resource_name(b_material))
        creator.set_mode(sdl.Enum("lerp"))
        creator.set_material_0(sdl.Material(mat0_res_name))
        creator.set_material_1(sdl.Material(mat1_res_name))
        if factor_res_name:
            creator.set_factor_map(sdl.Image(factor_res_name))
        else:
            factor = self.get_default_input_value(factor_input_index)
            if self.factor_type == 'FLOAT':
                factor = (factor, factor, factor)
            creator.set_factor(sdl.Spectrum(factor))
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.inputs.new(PhSurfaceMaterialSocket.bl_idname, "Material A")
        self.inputs.new(PhSurfaceMaterialSocket.bl_idname, "Material B")
        self.inputs.new(PhFloatFactorSocket.bl_idname, "Factor")
        self.inputs.new(PhColorSocket.bl_idname, "Factor")
        self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)

        self.width *= 1.2

        # owning_material = bpy.context.material
        # owning_node_tree = material.find_node_tree_from_material(owning_material)
        #
        # # Create a new default node if not found
        # default_node_name = PhBinaryMixedSurfaceNode.DEFAULT_NODE_NAME
        # default_node = owning_node_tree.get(default_node_name, None)
        # if default_node is None:
        #     default_node = owning_node_tree.nodes.new(PhPureAbsorberNode.bl_idname)
        #     default_node.name = default_node_name
        #     default_node.select = False
        #     default_node.hide = True
        #
        # # Link both input sockets to the default node
        # owning_node_tree.links.new(default_node.outputs[0], self.inputs[0])
        # owning_node_tree.links.new(default_node.outputs[0], self.inputs[1])

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, 'factor_type', text="")
