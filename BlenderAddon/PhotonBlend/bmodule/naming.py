import bpy


def get_decorated_name(name, **decorations):
	decorated_name = name
	if 'prefix' in decorations:
		decorated_name = decorations.get('prefix') + "$" + decorated_name
	if 'suffix' in decorations:
		decorated_name = decorated_name + "$" + decorations.get('suffix')

	return decorated_name


def get_mangled_mesh_name(b_mesh: bpy.types.Mesh, **decorations):
	return "G" + get_decorated_name(b_mesh.name, **decorations)


def get_mangled_material_name(b_material: bpy.types.Material, **decorations):
	return "M" + get_decorated_name(b_material.name, **decorations)


def get_mangled_object_name(b_object: bpy.types.Object, **decorations):
	return "O" + get_decorated_name(b_object.name, **decorations)

def get_mangled_emissive_image_name(b_material: bpy.types.Material, **decorations):
