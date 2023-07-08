from utility import blender
from bmodule import (
        cameras,
        exporter,
        lights,
        materials,
        renderer,
        world)

import bpy


@blender.register_class
class PhPhotonBlendPreferences(bpy.types.AddonPreferences):
    # This must match the add-on name, use '__package__' when defining this in a submodule of a python package.
    bl_idname = __name__.split(".")[0]

    installation_path: bpy.props.StringProperty(
        name="Installation Path",
        description="Path to the installation folder of Photon renderer.",
        subtype='DIR_PATH',
        default=""  # TODO: try to find it automatically
    )

    def draw(self, b_context):
        b_layout = self.layout
        b_layout.prop(self, "installation_path")


def include_module(module_manager):
    pass
