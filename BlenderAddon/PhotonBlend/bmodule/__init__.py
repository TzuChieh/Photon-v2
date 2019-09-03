from . import (
        cameras,
        exporter,
        lights,
        materials,
        renderer,
        world)


def include_module(module_manager):
    cameras.include_module(module_manager)
    exporter.include_module(module_manager)
    lights.include_module(module_manager)
    materials.include_module(module_manager)
    renderer.include_module(module_manager)
    world.include_module(module_manager)
