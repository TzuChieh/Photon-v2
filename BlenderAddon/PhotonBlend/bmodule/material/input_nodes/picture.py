from ..node_base import (
        PhMaterialNode,
        PhColorSocket,
        INPUT_CATEGORY)
from ....psdl.pysdl import (
        LdrPictureImageCreator,
        ConstantImageCreator,
        SDLReal,
        SDLString)
from ... import naming
from .... import utility
from ....psdl import sdlresource

import bpy

import shutil


class PhPictureNode(PhMaterialNode):
    bl_idname = "PH_PICTURE"
    bl_label = "Picture"
    node_category = INPUT_CATEGORY

    file_path: bpy.props.StringProperty(
        name="File",
        default="",
        subtype="FILE_PATH"
    )

    def to_sdl(self, b_material, sdlconsole):
        image_socket = self.outputs[0]
        image_res_name = naming.get_mangled_output_node_socket_name(image_socket, b_material)

        if self.file_path != "":
            creator = LdrPictureImageCreator()
            image_path = bpy.path.abspath(self.file_path)
            image_sdlri = sdlresource.SdlResourceIdentifier()
            image_sdlri.append_folder(PhPictureNode.bl_idname + "_pictures")
            image_sdlri.set_file(utility.get_filename(image_path))
            creator.set_image(SDLString(image_sdlri.get_identifier()))
            creator.set_sample_mode(SDLString("bilinear"))

            # copy the file to scene folder
            sdlconsole.create_resource_folder(image_sdlri)
            dst_path = utility.get_appended_path(sdlconsole.get_working_directory(),
                                                 image_sdlri.get_path())
            shutil.copyfile(image_path, dst_path)
        else:
            print("warning: picture node in material %s has no image file, result will be black" % b_material.name)

            creator = ConstantImageCreator()
            creator.set_value_type(SDLString("raw"))
            creator.set_value(SDLReal(0))

        creator.set_data_name(image_res_name)
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.outputs.new(PhColorSocket.bl_idname, PhColorSocket.bl_label)

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, "file_path")
