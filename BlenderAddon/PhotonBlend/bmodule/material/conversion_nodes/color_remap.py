from ..node_base import (
    PhMaterialConversionNode,
    PhColorSocket,
    PhFloatFactorSocket)
from psdl import sdl

import bpy


# Blender's CurveMapping table has 256 intervals. Export the same interval boundaries.
_NUM_LUT_INTERVALS = 256

# Base name for hidden ShaderNodeTree sidecars that own Blender's native RGB Curves node.
_CURVE_TREE_NAME = ".photon_internal_Color_Remap"

_CURVE_NODE_NAME = "RGB Curves"


def _bake_rgb_lut(b_mapping: bpy.types.CurveMapping):
    """
    Sample a standard Blender RGB `CurveMapping` for Photon.
    """
    b_mapping.update()
    # Blender stores curves as [Red, Green, Blue, Combined]
    curves = b_mapping.curves
    # [Combined] is applied before [Red, Green, Blue]
    combined_curve = curves[3]

    def sample_rgb(input_value):
        combined_value = b_mapping.evaluate(combined_curve, input_value)
        return tuple(b_mapping.evaluate(curves[channel], combined_value) for channel in range(3))

    sample_interval = 1.0 / _NUM_LUT_INTERVALS
    rgb_values = [sample_rgb(i * sample_interval) for i in range(_NUM_LUT_INTERVALS + 1)]

    # Use one LUT interval outside [0, 1] to approximate the out-of-range slopes
    below_zero = sample_rgb(0.0 - sample_interval)
    above_one = sample_rgb(1.0 + sample_interval)
    lower_slope = tuple((endpoint - extrapolated) / sample_interval for endpoint, extrapolated in zip(rgb_values[0], below_zero))
    upper_slope = tuple((extrapolated - endpoint) / sample_interval for endpoint, extrapolated in zip(rgb_values[-1], above_one))
    return rgb_values, lower_slope, upper_slope


class PhColorRemapNode(PhMaterialConversionNode):
    bl_idname = 'PH_COLOR_REMAP'
    bl_label = "Color Remap"
    bl_width_default = 240

    # Reuse Blender's native RGB Curves implementation for editing and serialization. Since custom
    # nodes cannot own a `CurveMapping``, a hidden `ShaderNodeTree`` sidecar stores the native node.
    curve_tree: bpy.props.PointerProperty(
        name="Curve Tree",
        type=bpy.types.NodeTree,
        options={'HIDDEN'})

    def _init_rgb_curve_node_if_not_exist(self):
        curve_tree = self.curve_tree
        if curve_tree is None or curve_tree.bl_idname != 'ShaderNodeTree':
            curve_tree = bpy.data.node_groups.new(_CURVE_TREE_NAME, 'ShaderNodeTree')
            self.curve_tree = curve_tree

        curve_node = curve_tree.nodes.get(_CURVE_NODE_NAME)
        if curve_node is None or curve_node.bl_idname != 'ShaderNodeRGBCurve':
            if curve_node is not None:
                curve_tree.nodes.remove(curve_node)
            curve_node = curve_tree.nodes.new('ShaderNodeRGBCurve')
            curve_node.name = _CURVE_NODE_NAME
        return curve_node

    def to_sdl(self, b_material, sdlconsole):
        if not self.outputs[0].is_linked:
            return

        color_input_index = 0
        input_res_name = self.get_linked_input_resource_name(b_material, color_input_index)
        if input_res_name is None:
            input_res_name = self.get_default_input_resource_name(b_material, color_input_index)
            input_creator = sdl.ConstantImageCreator()
            input_creator.set_data_name(input_res_name)
            input_creator.set_values(sdl.RealArray(self.get_default_input_value(color_input_index)))
            input_creator.set_color_space(sdl.Enum('LSRGB'))
            sdlconsole.queue_command(input_creator)

        rgb_values, lower_slope, upper_slope = _bake_rgb_lut(self._init_rgb_curve_node_if_not_exist().mapping)

        creator = sdl.ColorRemapImageCreator()
        creator.set_data_name(self.get_output_resource_name(b_material))
        creator.set_input(sdl.Image(input_res_name))
        creator.set_rgb_values(sdl.Vector3Array(rgb_values))
        creator.set_lower_extrapolation_slope(sdl.Vector3(lower_slope))
        creator.set_upper_extrapolation_slope(sdl.Vector3(upper_slope))

        factor_input_index = 1
        factor_res_name = self.get_linked_input_resource_name(b_material, factor_input_index)
        if factor_res_name is not None:
            creator.set_factor_map(sdl.Image(factor_res_name))
        else:
            creator.set_factor(sdl.Real(self.get_default_input_value(factor_input_index)))
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.inputs.new(PhColorSocket.bl_idname, PhColorSocket.bl_label)

        factor_socket = self.inputs.new(PhFloatFactorSocket.bl_idname, "Factor")
        factor_socket.default_value = 1.0

        self.outputs.new(PhColorSocket.bl_idname, PhColorSocket.bl_label)

        self._init_rgb_curve_node_if_not_exist()

    def draw_buttons(self, b_context, b_layout):
        b_layout.template_curve_mapping(
            self.curve_tree.nodes[_CURVE_NODE_NAME], "mapping", type='COLOR')

    def copy(self, source_node):
        source_node._init_rgb_curve_node_if_not_exist()

        # Duplicated Photon nodes must not edit the same `CurveMapping``
        self.curve_tree = source_node.curve_tree.copy()

    def free(self):
        curve_tree = self.curve_tree
        self.curve_tree = None
        if curve_tree is not None and curve_tree.name.startswith(_CURVE_TREE_NAME):
            bpy.data.node_groups.remove(curve_tree)
