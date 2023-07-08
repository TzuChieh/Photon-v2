import bpy

from abc import ABC, abstractmethod


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


# Helper decorator to register a module to Blender
def register_module(module_class):
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


# Helper decorator to register a addon class (bpy classes) to Blender
def register_class(bpy_addon_class):
    global module_manager

    if module_manager is None:
        print("ERROR: cannot register %s (module manager is not set)" % bpy_addon_class.__name__)
        return
    
    module_manager.add_module(BasicClassModule(bpy_addon_class))
    return bpy_addon_class
