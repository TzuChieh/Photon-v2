import bpy

from abc import ABC, abstractmethod


class BlenderModule(ABC):
    @abstractmethod
    def register(self):
        pass

    @abstractmethod
    def unregister(self):
        pass


class BlenderModuleManager:
    def __init__(self):
        self.modules = []

    def add_module(self, module: BlenderModule):
        self.modules.append(module)

    def register_all(self):
        for module in self.modules:
            module.register()

    def unregister_all(self):
        for module in reversed(self.modules):
            module.unregister()
