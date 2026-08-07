"""
@brief Main export functionalities.
"""
import utility

from utility import blender

from bmodule import (
    material,
    mesh,
    naming,
    scene,
    light,
    world,
    )

from psdl import sdl, sdlmapping, SdlConsole

import bpy
import mathutils

import time
from collections import Counter
from dataclasses import dataclass, field
from pathlib import Path


@dataclass
class _MeshInstanceBatch:
    source_actor_name: str
    instance_actor_name: str
    object_name: str
    b_world_matrices: list[mathutils.Matrix] = field(default_factory=list)


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
        print(f"exporting Photon scene to {scene_folder_path.resolve()}")

        self.__begin_time = time.time()

        utility.create_folder(scene_folder_path)

        self.__sdlconsole = SdlConsole(scene_folder_path, scene_name)
        self.__sdlconsole.start()

    def end(self):
        self.__sdlconsole.finish()

        elapsed_time = time.time() - self.__begin_time

        print("exporting complete (%f s)" % elapsed_time)
        print("-------------------------------------------------------------")

    # def exportRaw(self, rawText):
    # 	command = sdl.RawCommand()
    # 	command.append_string(rawText)
    # 	self.__sdlconsole.queue_command(command)

    def export_camera(self, b_camera_obj, b_scene):
        b_camera = b_camera_obj.data

        observer = None
        if b_camera.type == "PERSP":

            position, rot, scale = blender.to_photon_pos_rot_scale(b_camera_obj.matrix_world)
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

            render_width_px, render_height_px = blender.get_render_size_px(b_scene)
            render_aspect = render_width_px / render_height_px

            # Photon always treats sensor width as horizontal; Blender can sometimes fit vertically.
            sensor_width = b_camera.sensor_width
            if b_camera.sensor_fit == 'VERTICAL':
                sensor_width = b_camera.sensor_height * render_aspect
            elif b_camera.sensor_fit == 'AUTO' and render_aspect < 1.0:
                sensor_width *= render_aspect

            observer.set_sensor_width_mm(sdl.Real(sensor_width))
            observer.set_sensor_offset_mm(sdl.Real(b_camera.lens))

            if b_camera.ph_has_dof:
                observer.set_lens_radius_mm(sdl.Real(b_camera.ph_lens_radius_mm))
                observer.set_focal_distance_mm(sdl.Real(b_camera.ph_focal_meters * 1000))

        if observer is not None:
            observer.set_data_name(naming.get_mangled_camera_name(b_camera_obj))
            observer.set_display_name(sdl.String(b_camera_obj.name))
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
            sample_source.set_display_name(sdl.String("Sample Source"))
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
            visualizer.set_glossy_merge_begin_length_hint(sdl.Integer(b_scene.ph_render_glossy_merge_begin_length))
            visualizer.set_stochastic_view_sample_begin_length_hint(sdl.Integer(b_scene.ph_render_stochastic_view_sample_begin_length))

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
            visualizer.set_display_name(sdl.String("Visualizer"))
            if b_scene.ph_use_crop_window:
                visualizer.set_rect_x(sdl.Integer(b_scene.ph_crop_min_x))
                visualizer.set_rect_y(sdl.Integer(b_scene.ph_crop_min_y))
                visualizer.set_rect_w(sdl.Integer(b_scene.ph_crop_width))
                visualizer.set_rect_h(sdl.Integer(b_scene.ph_crop_height))

            self.get_sdlconsole().queue_command(visualizer)

    def export_options(self, b_scene):
        observer_name = naming.get_mangled_camera_name(b_scene.camera)

        render_session = sdl.SingleFrameRenderSessionOptionCreator()
        render_session.set_data_name("session")
        render_session.set_display_name(sdl.String("Render Session"))
        render_session.set_visualizer(sdl.String("@visualizer"))# HACK
        render_session.set_observer(sdl.String(f"@{observer_name}"))# HACK
        render_session.set_sample_source(sdl.String("@sample-source"))# HACK
        render_session.set_top_level_accelerator(sdl.Enum(b_scene.ph_top_level_accelerator))

        render_size_px = blender.get_render_size_px(b_scene)
        render_session.set_frame_size(sdl.Vector2(render_size_px))

        self.get_sdlconsole().queue_command(render_session)

    # TODO: write/flush commands to disk once a while (reducing memory usage)
    def export(self, b_depsgraph: bpy.types.Depsgraph):
        print("Input dependency graph mode (Exporter): %s" % str(b_depsgraph.mode))
        
        # Count mesh instances before exporting
        source_key_to_instance_counts = Counter()
        num_mesh_obj_instances = 0
        for _, b_obj_instance in scene.iter_mesh_obj_instances(b_depsgraph):
            b_mesh_obj = b_obj_instance.object
            num_mesh_obj_instances += 1
            source_key = mesh.export.get_mesh_obj_source_key(b_mesh_obj)
            if source_key is not None:
                source_key_to_instance_counts[source_key] += 1

        b_materials = scene.find_materials_from_mesh_obj_instances(b_depsgraph)
        b_light_objs = scene.find_light_objs(b_depsgraph)

        print(
            f"Exporter found {num_mesh_obj_instances} mesh object instances, "
            f"{len(b_materials)} materials, "
            f"and {len(b_light_objs)} light objects")

        # Exporting Blender data as SDL

        # TODO: export all cameras, not just the active one
        b_camera_obj = scene.find_active_camera_obj(b_depsgraph)
        self.export_camera(b_camera_obj, b_depsgraph.scene_eval)

        for b_material in b_materials:
            print("exporting material: " + b_material.name)
            material.to_sdl(b_material, self.get_sdlconsole())

        # Export each reusable mesh once and collect its transforms into one actor batch
        source_key_to_instance_batch = {}
        for depsgraph_index, b_obj_instance in scene.iter_mesh_obj_instances(b_depsgraph):
            b_mesh_obj = b_obj_instance.object
            source_key = mesh.export.get_mesh_obj_source_key(b_mesh_obj)
            num_instances = source_key_to_instance_counts.get(source_key, 0)
            if num_instances > 1 and mesh.export.can_mesh_obj_be_instance_source(b_mesh_obj):
                instance_batch = source_key_to_instance_batch.get(source_key)

                # Export source if not already exported
                if instance_batch is None:
                    print(f"exporting source for {num_instances} instances of mesh object: {b_mesh_obj.name}")
                    source_actor_name = mesh.export.mesh_obj_to_sdl_instance_source(
                        b_mesh_obj,
                        self.get_sdlconsole(),
                        name_suffix=naming.join_name_parts("source", depsgraph_index))
                    instance_actor_name = naming.get_mangled_actor_name(
                        b_mesh_obj, "instances", depsgraph_index)
                    instance_batch = _MeshInstanceBatch(
                        source_actor_name, instance_actor_name, b_mesh_obj.name)
                    source_key_to_instance_batch[source_key] = instance_batch

                instance_batch.b_world_matrices.append(b_obj_instance.matrix_world.copy())
            else:
                print(f"exporting mesh object: {b_mesh_obj.name}")
                mesh.export.mesh_obj_to_sdl_actor(
                    b_mesh_obj,
                    self.get_sdlconsole(),
                    b_world_matrix=b_obj_instance.matrix_world,
                    name_suffix=depsgraph_index)

        # Export each collected instance batch
        for instance_batch in source_key_to_instance_batch.values():
            mesh.export.queue_transformed_instance_actor(
                self.get_sdlconsole(),
                source_actor_name=instance_batch.source_actor_name,
                actor_name=instance_batch.instance_actor_name,
                display_name=instance_batch.object_name,
                b_world_matrices=instance_batch.b_world_matrices)

        for b_light_obj in b_light_objs:
            print(f"exporting light object: {b_light_obj.name}")
            light.export.light_object_to_sdl_actor(b_light_obj, self.get_sdlconsole())

        b_world_obj = b_depsgraph.scene_eval.world
        if b_world_obj is not None:
            print(f"exporting world object {b_world_obj.name}")
            world.export.world_to_sdl_actor(b_world_obj, self.get_sdlconsole())
