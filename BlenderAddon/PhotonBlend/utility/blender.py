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


class BlenderModuleManager:
    def __init__(self):
        self.modules = []

    def add_module(self, module: BlenderModule):
        self.modules.append(module)

    def add_class(self, clazz):
        self.add_module(BasicClassModule(clazz))

    def register_all(self):
        for module in self.modules:
            module.register()

    def unregister_all(self):
        for module in reversed(self.modules):
            module.unregister()
