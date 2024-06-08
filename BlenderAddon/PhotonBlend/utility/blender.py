"""
@brief General Blender-related helpers.
"""
import bpy
from mathutils import Vector, Quaternion, Matrix

from abc import ABC, abstractmethod
import math


def get_render_width_px(b_scene: bpy.types.Scene):
    return get_render_size_px(b_scene)[0]


def get_render_height_px(b_scene: bpy.types.Scene):
    return get_render_size_px(b_scene)[1]


def get_render_size_px(b_scene: bpy.types.Scene):

    # Check if the active camera wants to override the resolution. If not, use the resolution set
    # by Output panel
    b_active_camera = b_scene.camera.data
    if b_active_camera is not None and b_active_camera.ph_force_resolution:
        resolution_scale = b_active_camera.ph_resolution_percentage / 100.0
        resolution_x = int(b_active_camera.ph_resolution_x * resolution_scale)
        resolution_y = int(b_active_camera.ph_resolution_y * resolution_scale)
    else:
        resolution_scale = b_scene.render.resolution_percentage / 100.0
        resolution_x = int(b_scene.render.resolution_x * resolution_scale)
        resolution_y = int(b_scene.render.resolution_y * resolution_scale)

    return (resolution_x, resolution_y)


def get_render_threads(b_scene: bpy.types.Scene):
    # This will automatically adapt to what `threads_mode` specifies
    num_specified_threads = b_scene.render.threads

    num_reserved_threads = b_scene.ph_num_reserved_threads
    return max(num_specified_threads - num_reserved_threads, 1)


def blender_to_photon_quat():
	"""
	In Blender, its coordinate system is right-handed, x-right, z-up and y-front. This function returns
	a `Quaternion` to transform from Blender's to Photon's coordinate system (which is right-handed, x-right, 
	y-up and -z-front).
	"""
	return Quaternion((1.0, 0.0, 0.0), math.radians(-90.0))


def blender_to_photon_mat():
	"""
	Returns a 3x3 `Matrix` to transform from Blender's to Photon's coordinate system.
	"""
	# TODO: could be a constant (directly specify matrix elements)
	return blender_to_photon_quat().to_matrix()


def to_photon_vec3(b_vec3: Vector):
	"""
	Transform a `Vector` from Blender's to Photon's coordinate system.
	"""
	return blender_to_photon_mat() @ b_vec3


def to_photon_quat(b_quat: Quaternion):
	"""
	Transform a `Quaternion` from Blender's to Photon's coordinate system. Keep in mind that Blender's 
	`Quaternion` is in (w, x, y, z) order, and the `@` operator do not work like `Matrix` 
	(its `quat @ additional_quat` while matrix is `additional_mat @ mat`).
	"""
	return b_quat @ blender_to_photon_quat()


def to_photon_pos_rot_scale(b_matrix: Matrix):
	"""
	In Blender, its coordinate system is right-handed, x-right, z-up and y-front. This function takes a 
	matrix from Blender and returns decomposed transformations (position, rotation and scale) in Photon's 
	coordinate system (which is right-handed, x-right, y-up and -z-front).
	"""
	blender_to_photon = blender_to_photon_mat().to_4x4()

	pos, rot, scale = (blender_to_photon @ b_matrix).decompose()

	return pos, rot, scale


class BlenderModule(ABC):
    @abstractmethod
    def register(self):
        pass

    @abstractmethod
    def unregister(self):
        pass


class BasicClassModule(BlenderModule):
    def __init__(self, clazz):
        self.clazz = clazz

    def register(self):
        bpy.utils.register_class(self.clazz)

    def unregister(self):
        bpy.utils.unregister_class(self.clazz)

    def __eq__(self, other):
        return isinstance(other, type(self)) and self.clazz == other.clazz

    def __str__(self):
        return self.clazz.__name__


class BlenderModuleManager:
    def __init__(self):
        # Using list not set as we would like to retain the order of registration
        self.modules = []

    def add_module(self, module: BlenderModule):
        if module in self.modules:
            print("ERROR: module %s already registered" % module)
            return

        self.modules.append(module)

    def register_all(self):
        for module in self.modules:
            module.register()

    def unregister_all(self):
        for module in reversed(self.modules):
            module.unregister()


module_manager = None


def register_module(module_class):
    """
    @brief Helper decorator to register a module to Blender
    """
    global module_manager

    if module_manager is None:
        print("ERROR: cannot register %s (module manager is not set)" % module_class.__name__)
        return

    # Check for potential misuse
    if not issubclass(module_class, BlenderModule):
        print("ERROR: cannot register %s (not a BlenderModule type)" % module_class.__name__)
        return

    module_manager.add_module(module_class())
    return module_class


def register_class(bpy_addon_class):
    """
    @brief Helper decorator to register a addon class (bpy classes) to Blender.
    """
    global module_manager

    if module_manager is None:
        print("ERROR: cannot register %s (module manager is not set)" % bpy_addon_class.__name__)
        return
    
    module_manager.add_module(BasicClassModule(bpy_addon_class))
    return bpy_addon_class
