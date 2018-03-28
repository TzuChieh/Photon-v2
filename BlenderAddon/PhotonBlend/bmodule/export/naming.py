
def mangled_geometry_name(obj, name, suffix):
	return "geometry_" + obj.name + "_" + name + "_" + suffix


def mangled_material_name(obj, name, suffix):
	return "material_" + obj.name + "_" + name + "_" + suffix


def mangled_light_source_name(obj, name, suffix):
	return "light_source_" + obj.name + "_" + name + "_" + suffix


def mangled_actor_model_name(obj, name, suffix):
	return "actor_model_" + obj.name + "_" + name + "_" + suffix


def mangled_actor_light_name(obj, name, suffix):
	return "actor_light_" + obj.name + "_" + name + "_" + suffix
