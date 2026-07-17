from ..node_base import (
    PhMaterialInputNode,
    PhColorSocket,
    PhFloatFactorSocket,
    PhFloatValueSocket,
    PhFloatVectorSocket)
from psdl import sdl

import bpy


class PhNoiseInputNode(PhMaterialInputNode):
    bl_idname = 'PH_NOISE'
    bl_label = "Procedural Noise"
    bl_width_default = 170

    algorithm: bpy.props.EnumProperty(
        items=[
            ('perlin-fbm', "Perlin fBM", "Fractal Perlin noise", 0),
        ],
        name="Algorithm",
        description="Procedural noise algorithm",
        default='perlin-fbm'
    )

    dimensions: bpy.props.EnumProperty(
        items=[
            ('1d', "1D", "Use the first coordinate component", 1),
            ('2d', "2D", "Use the first two coordinate components", 2),
            ('3d', "3D", "Use the first three coordinate components", 3),
            ('4d', "4D", "Use the first four coordinate components", 4),
        ],
        name="Dimensions",
        description="Number of input coordinate components in the noise domain",
        default='3d'
    )

    normalize: bpy.props.BoolProperty(
        name="Normalize",
        description="Normalize output when supported by the selected algorithm",
        default=True
    )

    def _set_image_or_value(self, b_material, input_index, map_setter, value_setter):
        image_name = self.get_linked_input_resource_name(b_material, input_index)
        if image_name is not None:
            map_setter(sdl.Image(image_name))
        else:
            value_setter(sdl.Real(self.get_default_input_value(input_index)))

    def to_sdl(self, b_material, sdlconsole):
        linked_output_indices = [output_index for output_index, output_socket in enumerate(self.outputs) if output_socket.is_linked]

        for output_index in linked_output_indices:
            creator = sdl.NoiseImageCreator()
            creator.set_data_name(self.get_output_resource_name(b_material, output_index))
            creator.set_algorithm(sdl.Enum(self.algorithm))
            creator.set_dimensions(sdl.Enum(self.dimensions))
            creator.set_normalize(sdl.Bool(self.normalize))

            coordinates_name = self.get_linked_input_resource_name(b_material, 0)
            if coordinates_name is not None:
                creator.set_coordinates(sdl.Image(coordinates_name))

            self._set_image_or_value(b_material, 1, creator.set_frequency_map, creator.set_frequency)
            self._set_image_or_value(b_material, 2, creator.set_num_layers_map, creator.set_num_layers)
            self._set_image_or_value(b_material, 3, creator.set_amplitude_ratio_map, creator.set_amplitude_ratio)
            self._set_image_or_value(b_material, 4, creator.set_frequency_ratio_map, creator.set_frequency_ratio)
            self._set_image_or_value(b_material, 5, creator.set_warp_map, creator.set_warp)

            sdlconsole.queue_command(creator)

    def init(self, b_context):
        coordinates_socket = self.inputs.new(PhFloatVectorSocket.bl_idname, "Coordinates")
        coordinates_socket.link_only = True

        frequency_socket = self.inputs.new(PhFloatValueSocket.bl_idname, "Frequency")
        frequency_socket.default_value = 5.0

        num_layers_socket = self.inputs.new(PhFloatValueSocket.bl_idname, "Num Layers")
        num_layers_socket.default_value = 3.0

        amplitude_ratio_socket = self.inputs.new(PhFloatFactorSocket.bl_idname, "Amplitude Ratio")
        amplitude_ratio_socket.default_value = 0.5

        frequency_ratio_socket = self.inputs.new(PhFloatValueSocket.bl_idname, "Frequency Ratio")
        frequency_ratio_socket.default_value = 1.99

        warp_socket = self.inputs.new(PhFloatValueSocket.bl_idname, "Warp")
        warp_socket.default_value = 0.0

        self.outputs.new(PhFloatFactorSocket.bl_idname, "Value")
        self.outputs.new(PhColorSocket.bl_idname, "Color")

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, 'algorithm', text="")
        b_layout.prop(self, 'dimensions', text="")
        b_layout.prop(self, 'normalize')
