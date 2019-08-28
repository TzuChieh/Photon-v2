import bpy

from abc import ABC, abstractmethod


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
