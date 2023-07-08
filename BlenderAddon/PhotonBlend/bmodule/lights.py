from utility import settings, blender

import bpy
import sys


class PhLightPanel(bpy.types.Panel):
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "data"

    COMPATIBLE_ENGINES = {settings.renderer_id_name}

    @classmethod
    def poll(cls, context):
        render_settings = context.scene.render
        return (render_settings.engine in cls.COMPATIBLE_ENGINES and
                context.light)


@blender.register_class
class PH_LIGHT_PT_properties(PhLightPanel):
    """
    Specify and control light properties.
    """

    bl_label = "PR: Light"

    bpy.types.Light.ph_light_color_linear_srgb = bpy.props.FloatVectorProperty(
        name="Color",
        description="light color in linear sRGB",
        default=[1.0, 1.0, 1.0],
        min=0.0,
        max=1.0,
        subtype="COLOR",
        size=3
    )

    bpy.types.Light.ph_light_watts = bpy.props.FloatProperty(
        name="Watts",
        description="light energy in watts",
        default=100,
        min=0.0,
        max=sys.float_info.max
    )

    def draw(self, b_context):
        b_light = b_context.light
        layout = self.layout

        # HACK: relying on blender light type to change light data
        layout.prop(b_light, "type", expand=True)

        layout.prop(b_light, "ph_light_color_linear_srgb")
        layout.prop(b_light, "ph_light_watts")

        if b_light.type == "AREA":

            split = layout.split()

            col = split.column()
            col.prop(b_light, "shape", text="Shape")

            if b_light.shape == "SQUARE":

                col.prop(b_light, "size", text="Size")

            elif b_light.shape == "RECTANGLE":

                col.prop(b_light, "size", text="Width")
                col.prop(b_light, "size_y", text="Height")

            else:
                print("warning: unsupported area light shape %s" % b_light.shape)

        elif b_light.type == "POINT":

            # nothing to display
            pass

        else:
            print("warning: unsupported light type %s" % b_light.type)
