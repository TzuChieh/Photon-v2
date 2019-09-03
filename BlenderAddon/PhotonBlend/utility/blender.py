import bpy

from abc import ABC, abstractmethod


def _get_render_size_scale(b_scene: bpy.types.Scene):
    return b_scene.render.resolution_percentage / 100.0


def get_render_width_px(b_scene: bpy.types.Scene):
    original_width = b_scene.render.resolution_x
    return int(original_width * _get_render_size_scale(b_scene))


def get_render_height_px(b_scene: bpy.types.Scene):
    original_height = b_scene.render.resolution_y
    return int(original_height * _get_render_size_scale(b_scene))


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
