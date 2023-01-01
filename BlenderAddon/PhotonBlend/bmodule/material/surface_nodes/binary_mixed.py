from ..node_base import (
        PhMaterialNode,
        PhFloatFactorSocket,
        PhColorSocket,
        PhSurfaceMaterialSocket,
        SURFACE_MATERIAL_CATEGORY)
from psdl import sdl
from ... import naming
from .. import helper
from .pure_absorber import PhPureAbsorberNode

import bpy
import mathutils


class PhBinaryMixedSurfaceNode(PhMaterialNode):
    bl_idname = "PH_BINARY_MIXED_SURFACE"
    bl_label = "Binary Mixed Surface"
    node_category = SURFACE_MATERIAL_CATEGORY

    # DEFAULT_NODE_NAME = "__" + bl_idname + "_default_node"

    factor_type: bpy.props.EnumProperty(
        items=[
            ('FLOAT', "Float Factor", "Determine mix factor of all color channels together"),
            ('COLOR', "Color Factor", "Determine mix factor of each color channel")
        ],
        name="Factor Type",
        description="Type of mixing factor",
        default='FLOAT'
    )

    def to_sdl(self, b_material, sdlconsole):
        mat0_socket = self.inputs[0]
        mat1_socket = self.inputs[1]
        factor_socket = self.inputs[2] if self.factor_type == 'FLOAT' else self.inputs[3]
        surface_mat_socket = self.outputs[0]

        mat0_res_name = mat0_socket.get_from_res_name(b_material)
        mat1_res_name = mat1_socket.get_from_res_name(b_material)
        # TODO: use the default_value defined albedo
        if mat0_res_name is None or mat1_res_name is None:
            print("warning: material <%s>'s binary mixed surface node is incomplete" % b_material.name)
            return

        factor_res_name = factor_socket.get_from_res_name(b_material)
        if factor_res_name is None:
            image_creator = sdl.ConstantImageCreator()
            factor_res_name = naming.get_mangled_input_node_socket_name(factor_socket, b_material)
            image_creator.set_data_name(factor_res_name)
            factor = factor_socket.default_value
            if self.factor_type == 'FLOAT':
                image_creator.set_value(sdl.Real(factor))
            else:
                image_creator.set_value(sdl.Vector3(mathutils.Color((factor[0], factor[1], factor[2]))))
            image_creator.set_value_type(sdl.String("ecf-linear-srgb"))
            sdlconsole.queue_command(image_creator)

        creator = sdl.BinaryMixedSurfaceMaterialCreator()
        creator.set_data_name(naming.get_mangled_output_node_socket_name(surface_mat_socket, b_material))
        creator.set_material_0(sdl.Material(mat0_res_name))
        creator.set_material_1(sdl.Material(mat1_res_name))
        creator.set_factor(sdl.Image(factor_res_name))
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.inputs.new(PhSurfaceMaterialSocket.bl_idname, "Material A")
        self.inputs.new(PhSurfaceMaterialSocket.bl_idname, "Material B")
        self.inputs.new(PhFloatFactorSocket.bl_idname, "Factor")
        self.inputs.new(PhColorSocket.bl_idname, "Factor")
        self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)

        self.width *= 1.2

        # owning_material = bpy.context.material
        # owning_node_tree = helper.find_node_tree_from_material(owning_material)
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
        b_layout.prop(self, "factor_type", text="")
