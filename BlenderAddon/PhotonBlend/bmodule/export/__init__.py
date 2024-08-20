"""
@brief Main export functionalities.
"""
import utility

from utility import blender

from bmodule import (
    mesh,
    scene,
    light,
    world,
    )

from bmodule.material import nodes
from bmodule.mesh import triangle_mesh
from psdl import sdl, sdlmapping, SdlConsole
from utility import blender

import bpy
import mathutils

import math
import time
from pathlib import Path


class Exporter:
    def __init__(self, file_path):
        self.__file_path = Path(file_path)
        self.__sdlconsole = None

    # TODO: should not expose console
    def get_sdlconsole(self):
        return self.__sdlconsole

    def begin(self, scene_name):
        folder_path = self.__file_path.parent
        filename_without_ext = self.__file_path.stem
        scene_folder_path = folder_path / filename_without_ext

        print("-------------------------------------------------------------")
        print("exporting Photon scene to <%s>" % scene_folder_path)

        self.__begin_time = time.time()

        utility.create_folder(scene_folder_path)

        self.__sdlconsole = SdlConsole(scene_folder_path, scene_name)
        self.__sdlconsole.start()

    def end(self):
        self.__sdlconsole.finish()

        elapsed_time = time.time() - self.__begin_time

        print("exporting complete (%f s)" % elapsed_time)
        print("-------------------------------------------------------------")

    def export_material(self, b_material):
        # TODO: adhere to the material.export convention
        # FIXME: hack
        if b_material.photon.use_nodes:
            return nodes.to_sdl(b_material, self.get_sdlconsole())
        else:
            print("not using node tree")
            # BROKEN CODE
            # command = sdl.RawCommand()
            # command.append_string(ui.material.to_sdl(b_material, self.__sdlconsole, material_name))
            # self.__sdlconsole.queue_command(command)
            # return node.MaterialNodeTranslateResult()
            return None

    # def exportRaw(self, rawText):
    # 	command = sdl.RawCommand()
    # 	command.append_string(rawText)
    # 	self.__sdlconsole.queue_command(command)

    def export_camera(self, b_camera_object):
        b_camera = b_camera_object.data

        observer = None
        if b_camera.type == "PERSP":

            position, rot, scale = blender.to_photon_pos_rot_scale(b_camera_object.matrix_world)
            if abs(scale.x - 1.0) > 0.0001 or abs(scale.y - 1.0) > 0.0001 or abs(scale.z - 1.0) > 0.0001:
                print("warning: camera (%s) contains scale factor, ignoring" % b_camera.name)

            # Blender's camera initially pointing (0, 0, -1) with up (0, 1, 0) in its math.py system
            # (also note that Blender's quaternion works this way, does not require q*v*q').
            cam_dir = rot @ mathutils.Vector((0, 0, -1))
            cam_up_dir = rot @ mathutils.Vector((0, 1, 0))

            observer = sdl.SingleLensObserverCreator()
            observer.set_pos(sdl.Vector3(position))
            observer.set_dir(sdl.Vector3(cam_dir))
            observer.set_up_axis(sdl.Vector3(cam_up_dir))

            lens_unit = b_camera.lens_unit
            if lens_unit == 'FOV':
                fov_degrees = math.degrees(b_camera.angle)
                observer.set_fov_degrees(sdl.Real(fov_degrees))
            elif lens_unit == 'MILLIMETERS':
                sensor_width = b_camera.sensor_width
                focal_length = b_camera.lens
                observer.set_sensor_width_mm(sdl.Real(sensor_width))
                observer.set_sensor_offset_mm(sdl.Real(focal_length))
            else:
                print("warning: camera (%s) with lens unit %s is unsupported, not exporting" % (
                    b_camera.name, b_camera.lens_unit))

            if b_camera.ph_has_dof:
                observer.set_lens_radius_mm(sdl.Real(b_camera.ph_lens_radius_mm))
                observer.set_focal_distance_mm(sdl.Real(b_camera.ph_focal_meters * 1000))

        if observer is not None:
            observer.set_data_name("observer")
            self.get_sdlconsole().queue_command(observer)
        else:
            print("warning: camera (%s) type (%s) is unsupported, not exporting" % (b_camera.name, b_camera.type))

    def export_core_commands(self, b_scene):
        version_directive = sdl.VersionDirectiveCommand()
        self.get_sdlconsole().queue_command(version_directive)
        
        spp = b_scene.ph_render_num_spp
        filter_type = b_scene.ph_render_sample_filter_type
        sample_source_type = b_scene.ph_render_sample_source_type
        integrator_type = b_scene.ph_render_integrator_type
        scheduler_type = b_scene.ph_scheduler_type

        if sample_source_type == 'uniform-random':
            sample_source = sdl.UniformRandomSampleSourceCreator()
            sample_source.set_samples(sdl.Integer(spp))
        elif sample_source_type == 'stratified':
            sample_source = sdl.StratifiedSampleSourceCreator()
            sample_source.set_samples(sdl.Integer(spp))
        elif sample_source_type == 'halton':
            sample_source = sdl.HaltonSampleSourceCreator()
            sample_source.set_samples(sdl.Integer(spp))
            sample_source.set_permutation(sdl.Enum(b_scene.ph_render_halton_permutation))
            sample_source.set_sequence(sdl.Enum(b_scene.ph_render_halton_sequence))
        else:
            print("warning: sample source %s is not supported" % sample_source_type)
            sample_source = None

        if sample_source is not None:
            sample_source.set_data_name("sample-source")
            self.get_sdlconsole().queue_command(sample_source)

        if integrator_type in {'BVPT', 'BNEEPT', 'BVPTDL'}:
            visualizer = sdl.PathTracingVisualizerCreator()
            visualizer.set_sample_filter(sdlmapping.to_filter_enum(filter_type))
            visualizer.set_estimator(sdlmapping.to_integrator_enum(integrator_type))
            visualizer.set_scheduler(sdlmapping.to_scheduler_enum(scheduler_type))

        elif integrator_type in {'VPM', 'PPM', 'SPPM', 'PPPM'}:
            visualizer = sdl.PhotonMappingVisualizerCreator()
            visualizer.set_sample_filter(sdlmapping.to_filter_enum(filter_type))
            visualizer.set_mode(sdlmapping.to_integrator_enum(integrator_type))
            visualizer.set_num_photons(sdl.Integer(b_scene.ph_render_num_photons))
            visualizer.set_num_samples_per_pixel(sdl.Integer(b_scene.ph_render_num_spp_pm))
            visualizer.set_photon_radius(sdl.Real(b_scene.ph_render_kernel_radius))
            visualizer.set_glossy_merge_begin_length(sdl.Integer(b_scene.ph_render_glossy_merge_begin_length))
            visualizer.set_stochastic_view_sample_begin_length(sdl.Integer(b_scene.ph_render_stochastic_view_sample_begin_length))

            if integrator_type != 'VPM':
                visualizer.set_num_passes(sdl.Integer(b_scene.ph_render_num_passes))
        
        # elif render_method == "ATTRIBUTE":
        #     visualizer = sdl.AttributeRendererCreator()
        # elif render_method == "CUSTOM":
        #     custom_renderer_sdl_command = sdl.RawCommand()
        #     custom_renderer_sdl_command.append_string(b_scene.ph_render_custom_sdl)
        #     custom_renderer_sdl_command.append_string("\n")
        #     self.get_sdlconsole().queue_command(custom_renderer_sdl_command)
        else:
            print("warning: render method %s is not supported" % integrator_type)
            visualizer = None

        if visualizer is not None:
            visualizer.set_data_name("visualizer")
            if b_scene.ph_use_crop_window:
                visualizer.set_rect_x(sdl.Integer(b_scene.ph_crop_min_x))
                visualizer.set_rect_y(sdl.Integer(b_scene.ph_crop_min_y))
                visualizer.set_rect_w(sdl.Integer(b_scene.ph_crop_width))
                visualizer.set_rect_h(sdl.Integer(b_scene.ph_crop_height))

            self.get_sdlconsole().queue_command(visualizer)

    def export_options(self, b_scene):
        render_session = sdl.SingleFrameRenderSessionOptionCreator()
        render_session.set_data_name("session")
        render_session.set_visualizer(sdl.String("@visualizer"))# HACK
        render_session.set_observer(sdl.String("@observer"))# HACK
        render_session.set_sample_source(sdl.String("@sample-source"))# HACK
        render_session.set_top_level_accelerator(sdl.Enum(b_scene.ph_top_level_accelerator))

        render_size_px = blender.get_render_size_px(b_scene)
        render_session.set_frame_size(sdl.Vector2(render_size_px))

        self.get_sdlconsole().queue_command(render_session)

    # TODO: write/flush commands to disk once a while (reducing memory usage)
    def export(self, b_depsgraph: bpy.types.Depsgraph):
        print("Input dependency graph mode (Exporter): %s" % str(b_depsgraph.mode))
        
        b_camera_obj = scene.find_active_camera_object(b_depsgraph)
        b_mesh_objs = scene.find_mesh_objects(b_depsgraph)
        b_materials = scene.find_materials_from_mesh_objects(b_mesh_objs, b_depsgraph)
        b_light_objs = scene.find_light_objects(b_depsgraph)

        print("Exporter found %d mesh objects, %d materials, and %d light objects" % (
            len(b_mesh_objs),
            len(b_materials),
            len(b_light_objs)))

        # Exporting Blender data as SDL

        # TODO: export all cameras, not just the active one
        self.export_camera(b_camera_obj)

        for b_material in b_materials:
            print("exporting material: " + b_material.name)
            self.export_material(b_material)

        # TODO: Instancing; also see the comment in `mesh_object_to_sdl_actor()`, mesh data in mesh object may
        # have name collision even if they are actually different. How do we properly export mesh data should
        # be revisited
        for b_mesh_obj in b_mesh_objs:
            print(f"exporting mesh object: {b_mesh_obj.name}")
            mesh.export.mesh_object_to_sdl_actor(b_mesh_obj, self.get_sdlconsole())

        for b_light_obj in b_light_objs:
            print(f"exporting light object: {b_light_obj.name}")
            light.export.light_object_to_sdl_actor(b_light_obj, self.get_sdlconsole())

        b_world_obj = b_depsgraph.scene_eval.world
        if b_world_obj is not None:
            print(f"exporting world object {b_world_obj.name}")
            world.export.world_to_sdl_actor(b_world_obj, self.get_sdlconsole())
