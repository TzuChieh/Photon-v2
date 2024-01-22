from ..node_base import (
        PhMaterialInputNode,
        PhColorSocket)
from psdl import sdl
from bmodule import naming

import bpy


class PhPictureNode(PhMaterialInputNode):
    bl_idname = 'PH_PICTURE'
    bl_label = "Picture"
    bl_width_default = 160

    file_path: bpy.props.StringProperty(
        name="File",
        default="",
        subtype='FILE_PATH'
    )

    sample_mode: bpy.props.EnumProperty(
        items=[
            ('nearest', "Nearest", "", 0),
            ('bilinear', "Bilinear", "", 1),
        ],
        name="Sample Mode",
        description="Method used for sampling the picture.",
        default='bilinear'
    )

    wrap_mode: bpy.props.EnumProperty(
        items=[
            ('clamp-to-edge', "Clamp to Edge", "", 0),
            ('repeat', "Repeat", "", 1),
            ('flipped-clamp-to-edge', "Flipped Clamp to Edge", "", 2),
        ],
        name="Wrap Mode",
        description="Method used for wraping the picture when UV is out of [0, 1].",
        default='repeat'
    )

    color_space: bpy.props.EnumProperty(
        items=[
            ('auto', "Auto", "Use the color space information from file or make a best effort guess.", 0),
            ('XYZ', "CIE XYZ", "", 1),
            ('xyY', "CIE xyY", "", 2),
            ('LSRGB', "Linear sRGB", "", 3),
            ('SRGB', "sRGB", "", 4),
            ('ACES', "ACEScg", "", 5),
        ],
        name="Color Space",
        description="Color space of the picture.",
        default='auto'
    )

    is_raw_data: bpy.props.BoolProperty(
        name="Raw Data",
        description="Use the picture as raw data. No color space transformation will be done during rendering.",
        default=False
    )

    def to_sdl(self, b_material, sdlconsole):
        image_socket = self.outputs[0]
        image_res_name = naming.get_mangled_output_node_socket_name(image_socket, b_material) 

        # TODO: not bundle/copy the same file if already present

        if self.file_path != "":
            # Copy the image file to scene folder and obtain an identifier for it
            image_path = bpy.path.abspath(self.file_path)
            bundled_image_path = sdlconsole.bundle_file(image_path, PhPictureNode.bl_idname + "_pictures")
            image_identifier = sdl.ResourceIdentifier()
            image_identifier.set_bundled_path(bundled_image_path)

            creator = sdl.RasterFileImageCreator()
            creator.set_image_file(image_identifier)
            creator.set_sample_mode(sdl.Enum(self.sample_mode))
            creator.set_wrap_mode(sdl.Enum(self.wrap_mode))

            if self.is_raw_data:
                creator.set_is_color(sdl.Bool(False))
            
            if self.color_space != 'auto':
                creator.set_color_space(sdl.Enum(self.color_space))
        else:
            print("warning: picture node in material %s has no image file, result will be black" % b_material.name)

            creator = sdl.ConstantImageCreator()
            creator.set_values(sdl.RealArray([0]))

        creator.set_data_name(image_res_name)
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.outputs.new(PhColorSocket.bl_idname, PhColorSocket.bl_label)

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, 'file_path')
        b_layout.prop(self, 'sample_mode', text="Sample")
        b_layout.prop(self, 'wrap_mode', text="Wrap")

        b_layout.prop(self, 'is_raw_data')

        b_sub_col = b_layout.column(align=True)
        b_sub_col.enabled = not self.is_raw_data
        b_sub_col.prop(self, 'color_space', text="Color")
