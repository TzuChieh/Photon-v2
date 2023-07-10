from ..node_base import (
        PhMaterialInputNode,
        PhColorSocket)
from psdl import sdl
from bmodule import naming

import bpy


class PhPictureNode(PhMaterialInputNode):
    bl_idname = 'PH_PICTURE'
    bl_label = "Picture"

    file_path: bpy.props.StringProperty(
        name="File",
        default="",
        subtype="FILE_PATH"
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
            creator.set_sample_mode(sdl.Enum("bilinear"))
        else:
            print("warning: picture node in material %s has no image file, result will be black" % b_material.name)

            # TODO: properly handle color space
            creator = sdl.ConstantImageCreator()
            creator.set_color_space(sdl.String("LSRGB"))
            creator.set_value(sdl.Real(0))

        creator.set_data_name(image_res_name)
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.outputs.new(PhColorSocket.bl_idname, PhColorSocket.bl_label)

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, "file_path")
