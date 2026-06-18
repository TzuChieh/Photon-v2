from ..node_base import (
    PhMaterialGroupNode)
from psdl import sdl

import bpy


class PhGroupNode(PhMaterialGroupNode):
    bl_idname = 'PH_DEFAULT_GROUP'
    bl_label = "Group Node"

    idname_prefix = 'PH_GROUP_'

    def to_sdl(self, b_material, sdlconsole):
        creator = sdl.IdealMediumMaterialCreator()
        creator.set_data_name(self.get_output_resource_name(b_material))
        creator.set_absorption_coeff(sdl.Spectrum(self.absorption_coeff))

        sdlconsole.queue_command(creator)

    def init(self, b_context):
        tree = self.find_group_tree()
        if not tree:
            return
        
        # TODO: sockets
        # TODO: update

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, 'absorption_coeff', text="")

    @classmethod
    def find_group_tree(cls):
        def is_group_tree(tree):
            idname = getattr(tree, 'associated_node_idname', "")
            return idname == cls.bl_idname

        group_trees = [tree for tree in bpy.data.node_groups if is_group_tree(tree)]
        return group_trees[0] if len(group_trees) == 1 else None
