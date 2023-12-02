import utility

from utility import blender

from bmodule import (
        naming,
        material,
        scene,
        light)

from bmodule.material import nodes
from bmodule.mesh import triangle_mesh
from psdl import sdl, mapping
from psdl.sdlconsole import SdlConsole
from utility import blender
from bmodule.export import cycles_material

import bpy
import mathutils

import math


class Exporter:
    def __init__(self, file_path):
        self.__file_path = file_path
        self.__sdlconsole = None

    # TODO: should not expose console
    def get_sdlconsole(self):
        return self.__sdlconsole

    def begin(self, scene_name):
        file_path = self.__file_path
        folder_path = utility.get_folder_path(file_path)
        filename_without_ext = utility.get_filename_without_ext(file_path)
        scene_folder_path = folder_path + filename_without_ext + utility.path_separator()

        print("-------------------------------------------------------------")
        print("exporting Photon scene to <%s>" % scene_folder_path)

        utility.create_folder(scene_folder_path)

        self.__sdlconsole = SdlConsole(scene_folder_path, scene_name)
        self.__sdlconsole.start()

    def end(self):
        self.__sdlconsole.finish()

        print("exporting complete")
        print("-------------------------------------------------------------")

    def export_material(self, b_material):
        # DEBUG
        # print("name = %s" % b_material.name)
        # print("evaluated = %s" % b_material.ph_node_tree_name)
        # print("original = %s" % b_material.original.ph_node_tree_name)

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

    def export_light_actor(self, light_actor_name, emission_image_name, geometry_name, material_name, position, rotation, scale):

        creator = sdl.ModelLightActorCreator()
        creator.set_data_name(light_actor_name)
        creator.set_emitted_radiance(sdl.Image(emission_image_name))
        creator.set_geometry(sdl.Geometry(geometry_name))
        creator.set_material(sdl.Material(material_name))
        self.get_sdlconsole().queue_command(creator)   

        translator = sdl.CallTranslate()
        translator.set_target_name(light_actor_name)
        translator.set_amount(sdl.Vector3(position))
        self.get_sdlconsole().queue_command(translator)

        rotator = sdl.CallRotate()
        rotator.set_target_name(light_actor_name)
        rotator.set_rotation(sdl.Quaternion((rotation.x, rotation.y, rotation.z, rotation.w)))
        self.get_sdlconsole().queue_command(rotator)

        scaler = sdl.CallScale()
        scaler.set_target_name(light_actor_name)
        scaler.set_amount(sdl.Vector3(scale))
        self.get_sdlconsole().queue_command(scaler)

    def export_model_actor(self, model_actor_name, geometry_name, material_name, position, rotation, scale):
        
        creator = sdl.ModelActorCreator()
        creator.set_data_name(model_actor_name)
        creator.set_geometry(sdl.Geometry(geometry_name))
        creator.set_material(sdl.Material(material_name))
        self.get_sdlconsole().queue_command(creator)

        translator = sdl.CallTranslate()
        translator.set_target_name(model_actor_name)
        translator.set_amount(sdl.Vector3(position))
        self.get_sdlconsole().queue_command(translator)

        rotator = sdl.CallRotate()
        rotator.set_target_name(model_actor_name)
        rotator.set_rotation(sdl.Quaternion((rotation.x, rotation.y, rotation.z, rotation.w)))
        self.get_sdlconsole().queue_command(rotator)

        scaler = sdl.CallScale()
        scaler.set_target_name(model_actor_name)
        scaler.set_amount(sdl.Vector3(scale))
        self.get_sdlconsole().queue_command(scaler)

    # def exportRaw(self, rawText):
    # 	command = sdl.RawCommand()
    # 	command.append_string(rawText)
    # 	self.__sdlconsole.queue_command(command)

    def export_mesh_object(self, b_mesh_object: bpy.types.Object):
        b_mesh = b_mesh_object.data
        if b_mesh is None:
            print("warning: mesh object (%s) has no mesh data, not exporting" % b_mesh_object.name)
            return

        if len(b_mesh.materials) == 0:
            print("warning: mesh object (%s) has no material, not exporting" % b_mesh_object.name)
            return

        # Group faces with the same material, then export each material-faces pair as a Photon actor.

        b_mesh.calc_loop_triangles()
        if not b_mesh.has_custom_normals:
            b_mesh.calc_normals()
        else:
            b_mesh.calc_normals_split()

        # TODO: might be faster if using len(obj.material_slots()) for array size and simply store each loop tris array
        # TODO: material can link to mesh or object, distinguish them
        material_idx_loop_triangles_map = {}
        for b_loop_triangle in b_mesh.loop_triangles:
            # This index refers to material slots (their stack order in the UI).
            material_idx = b_loop_triangle.material_index

            if material_idx not in material_idx_loop_triangles_map.keys():
                material_idx_loop_triangles_map[material_idx] = []

            material_idx_loop_triangles_map[material_idx].append(b_loop_triangle)

        for material_idx in material_idx_loop_triangles_map.keys():
            b_material = b_mesh_object.material_slots[material_idx].material
            loop_triangles = material_idx_loop_triangles_map[material_idx]

            # A material slot can be empty, this check is necessary.
            if b_material is None:
                print("warning: no material is in mesh object %s's material slot %d, not exporting" % (
                    b_mesh_object.name, material_idx))
                continue

            # To ensure unique geometry name (which is required by Photon SDL), we use slot index as suffix. Note that
            # `bpy.types.Mesh.name` can be the same for different mesh data (even of it appears to have different names
            # in the outliner, tested in Blender 3.6). This is either a bug or due to support for geometry node, as geometry
            # can be procedurally generated and we cannot generate a good name for it. This is why we are using mesh object's
            # name as prefix. See Blender issue "Depsgraph returns wrong evaluated object name in bpy #100314" (https://projects.blender.org/blender/blender/issues/100314).
            # TODO: Whether this will affect object instancing need to be tested. We may export same mesh data multiple times now.
            geometry_name = naming.get_mangled_mesh_name(b_mesh, prefix=b_mesh_object.name, suffix=str(material_idx))
            material_name = naming.get_mangled_material_name(b_material)

            # Use the active one as the UV map for export.
            # TODO: support exporting multiple or zero UV maps/layers
            b_uv_layers = b_mesh.uv_layers
            b_active_uv_layer = b_uv_layers.active

            # TODO: support & check mesh without uv map
            # if len(b_mesh.uv_layers) == 0:
            #     print("warning: mesh (%s) has no uv maps, ignoring" % geometry_name)
            #     continue

            # TODO: support & check mesh without uv map
            # if b_active_uv_layer is None:
            #     print("warning: mesh (%s) has %d uv maps, but no one is active (no uv map will be exported)" % (
            #         geometry_name, len(b_uv_layers)))
            #     continue

            # TODO: support & check mesh with multiple uv maps
            if len(b_mesh.uv_layers) > 1:
                print("warning: mesh (%s) has %d uv maps, only the active one is exported" % (
                    geometry_name, len(b_uv_layers)))

            triangle_mesh.loop_triangles_to_sdl_triangle_mesh(
                geometry_name,
                self.get_sdlconsole(),
                loop_triangles,
                b_mesh.vertices,
                # b_active_uv_layer.data)
                b_active_uv_layer.data if b_active_uv_layer is not None else None,# HACK
                b_mesh.has_custom_normals)

            # creating actor (can be either model or light depending on emissivity)
            pos, rot, scale = utility.to_photon_pos_rot_scale(b_mesh_object.matrix_world)

            if material.helper.is_emissive(b_material):
                light_actor_name = naming.get_mangled_object_name(b_mesh_object, prefix="Emissive", suffix=str(material_idx))
                emission_image_name = material.helper.get_emission_image_res_name(b_material)
                self.export_light_actor(light_actor_name, emission_image_name, geometry_name, material_name, pos, rot, scale)
            else:
                model_actor_name = naming.get_mangled_object_name(b_mesh_object, suffix=str(material_idx))
                self.export_model_actor(model_actor_name, geometry_name, material_name, pos, rot, scale)

    def export_camera(self, b_camera_object):
        b_camera = b_camera_object.data

        observer = None
        if b_camera.type == "PERSP":

            position, rot, scale = utility.to_photon_pos_rot_scale(b_camera_object.matrix_world)
            if abs(scale.x - 1.0) > 0.0001 or abs(scale.y - 1.0) > 0.0001 or abs(scale.z - 1.0) > 0.0001:
                print("warning: camera (%s) contains scale factor, ignoring" % b_camera.name)

            # Blender's camera initially pointing (0, 0, -1) with up (0, 1, 0) in its math.py system
            # (also note that Blender's quaternion works this way, does not require q*v*q').
            cam_dir = rot @ mathutils.Vector((0, 0, -1))
            cam_up_dir = rot @ mathutils.Vector((0, 1, 0))

            observer = sdl.SingleLensObserverCreator()
            observer.set_position(sdl.Vector3(position))
            observer.set_direction(sdl.Vector3(cam_dir))
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

    def export_world(self, b_world):
        actor_name = "ph_" + b_world.name

        creator = None
        if b_world.ph_background_type == 'IMAGE' and b_world.ph_image_file_path != "":
            
            # TODO: not bundle/copy the same file if already present

            # Copy the envmap file to scene folder and obtain an identifier for it
            image_path = bpy.path.abspath(b_world.ph_image_file_path)
            bundled_image_path = self.get_sdlconsole().bundle_file(image_path, b_world.name + "_data")
            image_identifier = sdl.ResourceIdentifier()
            image_identifier.set_bundled_path(bundled_image_path)

            creator = sdl.ImageDomeActorCreator()
            creator.set_data_name(actor_name)
            creator.set_image_file(image_identifier)

        elif b_world.ph_background_type == 'PREETHAM':
            creator = sdl.PreethamDomeActorCreator()
            creator.set_data_name(actor_name)

            creator.set_turbidity(sdl.Real(b_world.ph_preetham_turbidity))
            creator.set_standard_time_24h(sdl.Real(b_world.ph_standard_time))
            creator.set_standard_meridian_degrees(sdl.Real(b_world.ph_standard_meridian))
            creator.set_site_latitude_degrees(sdl.Real(b_world.ph_latitude))
            creator.set_site_longitude_degrees(sdl.Real(b_world.ph_longitude))
            creator.set_julian_date(sdl.Integer(b_world.ph_julian_date))

        if creator is not None:
            creator.set_energy_scale(sdl.Real(b_world.ph_energy_scale))

            self.get_sdlconsole().queue_command(creator)

            rotation = sdl.DomeActorRotate()
            rotation.set_target_name(actor_name)
            rotation.set_axis(sdl.Vector3((0, 1, 0)))
            rotation.set_degrees(sdl.Real(b_world.ph_up_rotation))
            self.get_sdlconsole().queue_command(rotation)

    def export_core_commands(self, b_scene):
        version_directive = sdl.VersionDirectiveCommand()
        self.get_sdlconsole().queue_command(version_directive)
        
        spp = b_scene.ph_render_num_spp
        filter_type = b_scene.ph_render_sample_filter_type
        sample_source_type = b_scene.ph_render_sample_source_type
        integrator_type = b_scene.ph_render_integrator_type
        scheduler_type = b_scene.ph_scheduler_type

        sample_source = None
        if sample_source_type == 'RANDOM':
            sample_source = sdl.UniformRandomSampleSourceCreator()
            sample_source.set_samples(sdl.Integer(spp))
        elif sample_source_type == 'STRATIFIED':
            sample_source = sdl.StratifiedSampleSourceCreator()
            sample_source.set_samples(sdl.Integer(spp))
        elif sample_source_type == 'HALTON':
            sample_source = sdl.HaltonSampleSourceCreator()
            sample_source.set_samples(sdl.Integer(spp))
        else:
            print("warning: sample source %s is not supported" % sample_source_type)

        if sample_source is not None:
            sample_source.set_data_name("sample-source")
            self.get_sdlconsole().queue_command(sample_source)

        visualizer = None
        if integrator_type == 'BVPT' or integrator_type == 'BNEEPT' or integrator_type == 'BVPTDL':
            visualizer = sdl.PathTracingVisualizerCreator()
            visualizer.set_sample_filter(mapping.to_filter_enum(filter_type))
            visualizer.set_estimator(mapping.to_integrator_enum(integrator_type))
            visualizer.set_scheduler(mapping.to_scheduler_enum(scheduler_type))
        elif integrator_type == 'VPM':
            visualizer = sdl.PhotonMappingVisualizerCreator()
            visualizer.set_mode(mapping.to_integrator_enum(integrator_type))
            visualizer.set_num_photons(sdl.Integer(b_scene.ph_render_num_photons))
            visualizer.set_num_samples_per_pixel(sdl.Integer(b_scene.ph_render_num_spp_pm))
            visualizer.set_photon_radius(sdl.Real(b_scene.ph_render_kernel_radius))
        elif integrator_type == 'PPM' or integrator_type == 'SPPM':
            visualizer = sdl.PhotonMappingVisualizerCreator()
            visualizer.set_mode(mapping.to_integrator_enum(integrator_type))
            visualizer.set_num_photons(sdl.Integer(b_scene.ph_render_num_photons))
            visualizer.set_num_samples_per_pixel(sdl.Integer(b_scene.ph_render_num_spp_pm))
            visualizer.set_photon_radius(sdl.Real(b_scene.ph_render_kernel_radius))
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

        if visualizer is not None:
            visualizer.set_data_name("visualizer")
            if b_scene.ph_use_crop_window:
                visualizer.set_rect_x(sdl.Integer(b_scene.ph_crop_min_x))
                visualizer.set_rect_y(sdl.Integer(b_scene.ph_crop_min_y))
                visualizer.set_rect_w(sdl.Integer(b_scene.ph_crop_width))
                visualizer.set_rect_h(sdl.Integer(b_scene.ph_crop_height))

            self.get_sdlconsole().queue_command(visualizer)

    def export_options(self, b_scene):
        top_level_accelerator = None
        if b_scene.ph_top_level_accelerator == 'BF':
            top_level_accelerator = sdl.Enum("brute-force")
        elif b_scene.ph_top_level_accelerator == 'BVH':
            top_level_accelerator = sdl.Enum("bvh")
        elif b_scene.ph_top_level_accelerator == 'IKD':
            top_level_accelerator = sdl.Enum("indexed-kd-tree")

        render_session = sdl.SingleFrameRenderSessionOptionCreator()
        render_session.set_data_name("session")
        render_session.set_visualizer(sdl.String("@visualizer"))# HACK
        render_session.set_observer(sdl.String("@observer"))# HACK
        render_session.set_sample_source(sdl.String("@sample-source"))# HACK
        render_session.set_top_level_accelerator(top_level_accelerator)# HACK

        render_size_px = blender.get_render_size_px(b_scene)
        render_session.set_frame_size(sdl.Vector2(render_size_px))

        self.get_sdlconsole().queue_command(render_session)

    # TODO: write/flush commands to disk once a while (reducing memory usage)
    def export(self, b_depsgraph: bpy.types.Depsgraph):
        print("Input dependency graph mode (Exporter): %s" % str(b_depsgraph.mode))
        
        b_camera_object = scene.find_active_camera_object(b_depsgraph)
        b_mesh_objects = scene.find_mesh_objects(b_depsgraph)
        b_materials = scene.find_materials_from_mesh_objects(b_mesh_objects, b_depsgraph)
        b_light_objects = scene.find_light_objects(b_depsgraph)

        print("Exporter found %d mesh objects, %d materials, and %d light objects" % (
            len(b_mesh_objects),
            len(b_materials),
            len(b_light_objects)))

        # Exporting Blender data as SDL

        # TODO: export all cameras, not just the active one
        self.export_camera(b_camera_object)

        for b_material in b_materials:
            print("exporting material: " + b_material.name)
            self.export_material(b_material)

        # TODO: Instancing; also see the comment in `export_mesh_object()`, mesh data in mesh object may have name collision
        # even if they are actually different. How do we properly export mesh data should be revisited.
        for b_mesh_object in b_mesh_objects:
            print("exporting mesh object: " + b_mesh_object.name)
            self.export_mesh_object(b_mesh_object)

        for b_light_object in b_light_objects:
            print("exporting light object: " + b_light_object.name)
            light.light_object_to_sdl_actor(b_light_object, self.get_sdlconsole())

        b_world = b_depsgraph.scene_eval.world
        self.export_world(b_world)
