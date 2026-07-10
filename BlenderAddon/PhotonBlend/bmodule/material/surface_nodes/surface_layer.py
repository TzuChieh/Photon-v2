from ..node_base import (
        PhSurfaceMaterialNode,
        PhSurfaceLayerSocket,
        PhFloatFactorSocket,
        PhIorNSocket,
        PhIorKSocket,
        PhLayerThicknessSocket,
        PhHenyeyGreensteinGSocket,
        PhVolumeAbsorptionSocket,
        PhVolumeScatteringSocket)
from psdl import sdl


class PhSurfaceLayerNode(PhSurfaceMaterialNode):
    bl_idname = 'PH_SURFACE_LAYER'
    bl_label = "Surface Layer"

    def to_sdl(self, b_material, sdlconsole):
        pass

    def init(self, b_context):
        self.inputs.new(PhFloatFactorSocket.bl_idname, "Roughness")
        self.inputs.new(PhIorNSocket.bl_idname, "IoR N")
        self.inputs.new(PhIorKSocket.bl_idname, "IoR K")
        self.inputs.new(PhLayerThicknessSocket.bl_idname, "Thickness")
        self.inputs.new(PhHenyeyGreensteinGSocket.bl_idname, "G")
        self.inputs.new(PhVolumeAbsorptionSocket.bl_idname, "Sigma A")
        self.inputs.new(PhVolumeScatteringSocket.bl_idname, "Sigma S")
        self.outputs.new(PhSurfaceLayerSocket.bl_idname, PhSurfaceLayerSocket.bl_label)

    def make_cached_packet_command(self, b_material, packet_name):
        packet = sdl.CachedPacketCommand()
        packet.set_data_name(packet_name)

        roughness_img_name = self.get_linked_input_resource_name(b_material, 0)
        if roughness_img_name is not None:
            packet.set_input('roughness-map', sdl.Image(roughness_img_name))
        else:
            packet.set_input('roughness', sdl.Real(self.get_default_input_value(0)))

        ior_n_img_name = self.get_linked_input_resource_name(b_material, 1)
        if ior_n_img_name is not None:
            packet.set_input('ior-n-map', sdl.Image(ior_n_img_name))
        else:
            packet.set_input('ior-n', sdl.Spectrum(self.get_default_input_value(1)))

        ior_k_img_name = self.get_linked_input_resource_name(b_material, 2)
        if ior_k_img_name is not None:
            packet.set_input('ior-k-map', sdl.Image(ior_k_img_name))
        else:
            packet.set_input('ior-k', sdl.Spectrum(self.get_default_input_value(2)))

        depth_img_name = self.get_linked_input_resource_name(b_material, 3)
        if depth_img_name is not None:
            packet.set_input('depth-map', sdl.Image(depth_img_name))
        else:
            packet.set_input('depth', sdl.Real(self.get_default_input_value(3)))

        g_img_name = self.get_linked_input_resource_name(b_material, 4)
        if g_img_name is not None:
            packet.set_input('g-map', sdl.Image(g_img_name))
        else:
            packet.set_input('g', sdl.Real(self.get_default_input_value(4)))

        sigma_a_img_name = self.get_linked_input_resource_name(b_material, 5)
        if sigma_a_img_name is not None:
            packet.set_input('sigma-a-map', sdl.Image(sigma_a_img_name))
        else:
            packet.set_input('sigma-a', sdl.Spectrum(self.get_default_input_value(5)))

        sigma_s_img_name = self.get_linked_input_resource_name(b_material, 6)
        if sigma_s_img_name is not None:
            packet.set_input('sigma-s-map', sdl.Image(sigma_s_img_name))
        else:
            packet.set_input('sigma-s', sdl.Spectrum(self.get_default_input_value(6)))

        return packet
