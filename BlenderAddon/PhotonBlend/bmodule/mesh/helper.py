import bpy


def force_mesh_object_subdiv_level(b_mesh_object: bpy.types.Object, original_settings, level):
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
        original_settings[b_mesh_object.name] = settings


def restore_mesh_object_subdiv_level(b_mesh_object: bpy.types.Object, original_settings):
    if b_mesh_object.name not in original_settings:
        print("warning: cannot find mesh object <%s>'s original settings" % b_mesh_object.name)
        return

    settings = original_settings[b_mesh_object.name]
    for modifier_name, levels in settings.items():
        b_modifier = b_mesh_object.modifiers[modifier_name]
        b_modifier.levels = levels[0]
        b_modifier.render_levels = levels[1]


def delete_mesh_object(b_mesh_object: bpy.types.Object):
    bpy.ops.object.delete({"selected_objects": [b_mesh_object]})
