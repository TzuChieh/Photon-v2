from utility import settings, blender

import bpy


class PhLightPanel(bpy.types.Panel):
    bl_space_type = 'PROPERTIES'
    bl_region_type = 'WINDOW'
    bl_context = 'data'

    @classmethod
    def poll(cls, context):
        render_settings = context.scene.render
        return (render_settings.engine in settings.photon_engines and
                context.light)


@blender.register_class
class PH_LIGHT_PT_properties(PhLightPanel):
    """
    Specify and control light properties.
    """
    bl_label = "PR: Properties"

    def draw(self, b_context):
        b_light = b_context.light
        b_layout = self.layout

        # HACK: relying on blender light type to change light data, ideally we want our own chooser
        b_layout.row().prop(b_light, 'type', expand=True)

        col = b_layout.column()
        col.prop(b_light.photon, 'color_linear_srgb')
        col.prop(b_light.photon, 'watts')
        col.separator()

        if b_light.type == 'AREA':
            col.prop(b_light, 'shape', text="Shape")

            if b_light.shape == 'SQUARE':
                col.prop(b_light, 'size', text="Size")

            elif b_light.shape == 'RECTANGLE':
                col.prop(b_light, 'size', text="Width")
                col.prop(b_light, 'size_y', text="Height")

            else:
                print("warning: unsupported area light shape %s" % b_light.shape)

        elif b_light.type == 'POINT':
            col.prop(b_light, 'shadow_soft_size', text="Radius")

        else:
            print("warning: unsupported light type %s" % b_light.type)

@blender.register_class
class PH_LIGHT_PT_advanced(PhLightPanel):
    """
    Advanced light properties.
    """
    bl_label = "PR: Advanced"

    def draw(self, b_context):
        b_light = b_context.light
        layout = self.layout

        layout.prop(b_light.photon, 'directly_visible')

        if b_light.type == 'POINT':
            layout.prop(b_light.photon, 'point_light_bsdf_sample')
        else:
            layout.prop(b_light.photon, 'bsdf_sample')

        layout.prop(b_light.photon, 'direct_sample')
        layout.prop(b_light.photon, 'emission_sample')
