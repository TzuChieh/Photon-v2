"""
@brief Material extensions in Blender for Photon renderer.
"""
from bmodule.material import (
    properties,
    nodes,
    node_base,
    )


def to_sdl(b_material, sdlconsole):
    # TODO: Move this facade implementation to material.export.
    # FIXME: hack
    if b_material.photon.use_nodes:
        return nodes.to_sdl(b_material, sdlconsole)
    else:
        print('not using node tree')
        # BROKEN CODE
        # command = sdl.RawCommand()
        # command.append_string(ui.material.to_sdl(b_material, sdlconsole, material_name))
        # sdlconsole.queue_command(command)
        # return node.MaterialNodeTranslateResult()
        return None
