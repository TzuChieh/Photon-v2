"""
@brief General mesh-related helpers.
"""
import bpy


def mesh_object_force_subdiv_level(b_mesh_object: bpy.types.Object, level, out_original_settings):
    """
    Enforce a subdivision level if the object has a subdivision surface modifier. Original settings are 
    recorded and can be used to restore to previous state.
    """
    settings = {}
    for b_modifier in b_mesh_object.modifiers:
        if b_modifier.type == 'SUBSURF':
            # Record the levels so that we can restore them later
            settings[b_modifier.name] = (b_modifier.levels, b_modifier.render_levels)

            # Force specific subdivision level by making both level options equal
            if level == 'VIEWPORT' or level == 'PREVIEW':
                b_modifier.render_levels = b_modifier.levels
            elif level == 'RENDER':
                b_modifier.levels = b_modifier.render_levels
            else:
                b_modifier.levels = level
                b_modifier.render_levels = level

    if settings:
        out_original_settings[b_mesh_object.name] = settings


def restore_mesh_object_subdiv_level(b_mesh_object: bpy.types.Object, original_settings):
    """
    Restore object subdivision level from the records.
    """
    if b_mesh_object.name not in original_settings:
        return

    settings = original_settings[b_mesh_object.name]
    for modifier_name, levels in settings.items():
        b_modifier = b_mesh_object.modifiers[modifier_name]
        b_modifier.levels = levels[0]
        b_modifier.render_levels = levels[1]


def mesh_object_autosmooth_to_edgesplit(b_mesh_object: bpy.types.Object, out_original_settings):
    b_mesh = b_mesh_object.data

    # Custom normals will override auto smooth
    if not b_mesh.use_auto_smooth or b_mesh.has_custom_normals:
        return

    MODIFIER_NAME = "__mesh$object$autosmooth$to$edgesplit"

    b_modifier = b_mesh_object.modifiers.new(name=MODIFIER_NAME, type='EDGE_SPLIT')
    b_modifier.split_angle = b_mesh.auto_smooth_angle
    b_modifier.use_edge_angle = True
    b_modifier.use_edge_sharp = True

    out_original_settings[b_mesh_object.name] = MODIFIER_NAME


def restore_mesh_object_autosmooth(b_mesh_object: bpy.types.Object, original_settings):
    if b_mesh_object.name not in original_settings:
        return

    modifier_name = original_settings[b_mesh_object.name]
    b_modifier = b_mesh_object.modifiers[modifier_name]
    b_mesh_object.modifiers.remove(b_modifier)


def delete_mesh_object(b_mesh_object: bpy.types.Object):
    # Delete with an overridden context
    bpy.ops.object.delete({"selected_objects": [b_mesh_object]})