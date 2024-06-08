from utility import settings, blender

import bpy
from bl_ui import (
    properties_data_mesh,
    )


class PH_VIEW3D_MT_photon_extras(bpy.types.Menu):
    """
    Menu for extra Photon meshes.
    """
    bl_idname = 'PH_VIEW3D_MT_photon_extras'
    bl_label = "Photon Extras"

    def draw(self, b_context):
        b_layout = self.layout
        b_layout.operator_context = 'INVOKE_REGION_WIN'

        b_layout.operator('photon.add_menger_sponge', text="Menger Sponge")


def menu_func_photon_extras(self, b_context):
    """
    Add entries to "Add -> Mesh" menu.
    """
    b_layout = self.layout
    b_layout.operator_context = 'INVOKE_REGION_WIN'

    b_layout.separator()
    b_layout.menu('PH_VIEW3D_MT_photon_extras', text="Photon Extras", icon='MESH_CUBE')


@blender.register_module
class MeshModule(blender.BlenderModule):
    """
    For mesh functionalities that need a custom registration order. 
    """
    def register(self):
        bpy.utils.register_class(PH_VIEW3D_MT_photon_extras)

        # Tell UI Panels that they are compatible
        properties_data_mesh.DATA_PT_custom_props_mesh.COMPAT_ENGINES.add(settings.render_engine_idname)

        bpy.types.VIEW3D_MT_mesh_add.append(menu_func_photon_extras)

    def unregister(self):
        bpy.types.VIEW3D_MT_mesh_add.remove(menu_func_photon_extras)

        properties_data_mesh.DATA_PT_custom_props_mesh.COMPAT_ENGINES.remove(settings.render_engine_idname)

        bpy.utils.unregister_class(PH_VIEW3D_MT_photon_extras)
