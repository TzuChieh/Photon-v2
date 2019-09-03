from . import (
        cameras,
        exporter,
        lights,
        materials,
        renderer,
        world)

import bpy


class PhPhotonBlendPreferences(bpy.types.AddonPreferences):
    # This must match the add-on name, use '__package__' when defining this in a submodule of a python package.
    bl_idname = __name__.split(".")[0]

    executable_path: bpy.props.StringProperty(
        name="Photon Executable",
        description="Path to the executable of Photon renderer.",
        subtype='FILE_PATH',
        default=""  # TODO: try to find it automatically
    )

    def draw(self, b_context):
        b_layout = self.layout
        b_layout.prop(self, "executable_path")


def include_module(module_manager):
    module_manager.add_class(PhPhotonBlendPreferences)

    cameras.include_module(module_manager)
    exporter.include_module(module_manager)
    lights.include_module(module_manager)
    materials.include_module(module_manager)
    renderer.include_module(module_manager)
    world.include_module(module_manager)
