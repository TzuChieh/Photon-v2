from ... import utility
from .. import (
        naming,
        material,
        scene,
        light)
from ..material import nodes
from ..mesh import triangle_mesh
from ...psdl import sdlresource
from ...psdl.cmd import RawCommand
from ...psdl.pysdl import (
    PinholeReceiverCreator,
    SDLReal,
    SDLVector3,
    LightActorCreator,
    SDLLightSource,
    LightActorTranslate,
    LightActorRotate,
    SDLQuaternion,
    LightActorScale,
    ModelActorCreator,
    SDLGeometry,
    SDLMaterial,
    ModelActorTranslate,
    ModelActorRotate,
    ModelActorScale,
    ModelLightSourceCreator,
    SDLImage,
    ThinLensReceiverCreator,
    DomeActorCreator,
    SDLString,
    DomeActorRotate,
    UniformRandomSampleGeneratorCreator,
    StratifiedSampleGeneratorCreator,
    HaltonSampleGeneratorCreator,
    SDLInteger,
    EqualSamplingRendererCreator,
    PmRendererCreator,
    AttributeRendererCreator,
    CookSettingsOptionCreator,
    EngineOptionCreator)
from ...psdl.sdlconsole import SdlConsole
from ...utility import meta, blender
from . import cycles_material

import bpy
import mathutils

import math
import shutil


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
            # command = RawCommand()
            # command.append_string(ui.material.to_sdl(b_material, self.__sdlconsole, material_name))
            # self.__sdlconsole.queue_command(command)
            # return node.MaterialNodeTranslateResult()
            return None

    def export_actor_light(self, actorLightName, lightSourceName, geometryName, materialName, position, rotation, scale):

        # TODO: check non-uniform scale

        position = self.__blendToPhotonVector(position)
        rotation = self.__blendToPhotonQuaternion(rotation)
        scale = self.__blendToPhotonVector(scale)

        if lightSourceName is not None:
            creator = LightActorCreator()
            creator.set_data_name(actorLightName)
            creator.set_light_source(SDLLightSource(lightSourceName))
            self.__sdlconsole.queue_command(creator)
        else:
            print("warning: expecting a none light source name for actor-light %s, not exporting" % actorLightName)
            return

        # if geometryName != None:
        # 	command.append_string("[geometry geometry %s] " %("\"@" + geometryName + "\""))
        #
        # if materialName != None:
        # 	command.append_string("[material material %s] " %("\"@" + materialName + "\""))

        translator = LightActorTranslate()
        translator.set_target_name(actorLightName)
        translator.set_factor(SDLVector3(position))
        self.__sdlconsole.queue_command(translator)

        rotator = LightActorRotate()
        rotator.set_target_name(actorLightName)
        rotator.set_factor(SDLQuaternion((rotation.x, rotation.y, rotation.z, rotation.w)))
        self.__sdlconsole.queue_command(rotator)

        scaler = LightActorScale()
        scaler.set_target_name(actorLightName)
        scaler.set_factor(SDLVector3(scale))
        self.__sdlconsole.queue_command(scaler)

    def export_actor_model(self, actorModelName, geometryName, materialName, position, rotation, scale):
        if (
            actorModelName is None or
            geometryName is None or
            materialName is None
        ):
            print("warning: no name should be none, not exporting")
            return

        position = self.__blendToPhotonVector(position)
        rotation = self.__blendToPhotonQuaternion(rotation)
        scale = self.__blendToPhotonVector(scale)

        creator = ModelActorCreator()
        creator.set_data_name(actorModelName)
        creator.set_geometry(SDLGeometry(geometryName))
        creator.set_material(SDLMaterial(materialName))
        self.__sdlconsole.queue_command(creator)

        translator = ModelActorTranslate()
        translator.set_target_name(actorModelName)
        translator.set_factor(SDLVector3(position))
        self.__sdlconsole.queue_command(translator)

        rotator = ModelActorRotate()
        rotator.set_target_name(actorModelName)
        rotator.set_factor(SDLQuaternion((rotation.x, rotation.y, rotation.z, rotation.w)))
        self.__sdlconsole.queue_command(rotator)

        scaler = ModelActorScale()
        scaler.set_target_name(actorModelName)
        scaler.set_factor(SDLVector3(scale))
        self.__sdlconsole.queue_command(scaler)

    # def exportRaw(self, rawText):
    # 	command = RawCommand()
    # 	command.append_string(rawText)
    # 	self.__sdlconsole.queue_command(command)

    def __blendToPhotonVector(self, blenderVector):
        photonVector = mathutils.Vector((blenderVector.y,
                                         blenderVector.z,
                                         blenderVector.x))
        return photonVector

    def __blendToPhotonQuaternion(self, blenderQuaternion):
        # initializer is like mathutils.Quaternion(w, x, y, z)
        photonQuaternion = mathutils.Quaternion((blenderQuaternion.w,
                                                 blenderQuaternion.y,
                                                 blenderQuaternion.z,
                                                 blenderQuaternion.x))
        return photonQuaternion

    def export_mesh_object(self, b_mesh_object: bpy.types.Object):
        b_mesh = b_mesh_object.data

        if b_mesh is None:
            print("warning: mesh object (%s)　has no mesh data, not exporting" % b_mesh_object.name)
            return

        if len(b_mesh.materials) == 0:
            print("warning: mesh object (%s) has no material, not exporting" % b_mesh_object.name)
            return

        # Group faces with the same material, then export each face-material pair as a Photon-v2's actor.

        b_mesh.calc_loop_triangles()
        if not b_mesh.has_custom_normals:
            b_mesh.calc_normals()
        else:
            b_mesh.calc_normals_split()

        # TODO: might be faster if using len(obj.material_slots()) for array size and simply store each loop tris array
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

            # Using slot index as suffix so we can ensure unique geometry names (required by Photon SDL).
            geometry_name = naming.get_mangled_mesh_name(b_mesh, suffix=str(material_idx))
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
                self.__sdlconsole,
                loop_triangles,
                b_mesh.vertices,
                # b_active_uv_layer.data)
                b_active_uv_layer.data if b_active_uv_layer is not None else None,# HACK
                b_mesh.has_custom_normals)

            # creating actor (can be either model or light depending on emissivity)
            pos, rot, scale = b_mesh_object.matrix_world.decompose()

            if material.helper.is_emissive(b_material):
                light_source_name = geometry_name
                creator = ModelLightSourceCreator()
                creator.set_data_name(light_source_name)
                creator.set_emitted_radiance(SDLImage(material.helper.get_emission_image_res_name(b_material)))
                creator.set_geometry(SDLGeometry(geometry_name))
                creator.set_material(SDLMaterial(material_name))
                self.get_sdlconsole().queue_command(creator)

                actor_light_name = naming.get_mangled_object_name(b_mesh_object, suffix=str(material_idx))
                self.export_actor_light(actor_light_name, light_source_name, geometry_name, material_name, pos, rot, scale)
            else:
                actor_model_name = naming.get_mangled_object_name(b_mesh_object, suffix=str(material_idx))
                self.export_actor_model(actor_model_name, geometry_name, material_name, pos, rot, scale)

    def export_camera(self, b_camera_object):
        b_camera = b_camera_object.data
        b_scene = bpy.context.scene

        # Check if this camera is the active one and override resolution if so;
        # otherwise use the resolution settings carried by camera itself
        # (this behavior can be disabled via <ph_force_resolution>)
        if b_camera.name == b_scene.camera.data.name and not b_camera.ph_force_resolution:
            resolution_scale = b_scene.render.resolution_percentage / 100.0
            resolution_x = int(b_scene.render.resolution_x * resolution_scale)
            resolution_y = int(b_scene.render.resolution_y * resolution_scale)
        else:
            resolution_scale = b_camera.ph_resolution_percentage / 100.0
            resolution_x = int(b_camera.ph_resolution_x * resolution_scale)
            resolution_y = int(b_camera.ph_resolution_y * resolution_scale)

        camera = None
        if b_camera.type == "PERSP":

            pos, rot, scale = b_camera_object.matrix_world.decompose()
            if abs(scale.x - 1.0) > 0.0001 or abs(scale.y - 1.0) > 0.0001 or abs(scale.z - 1.0) > 0.0001:
                print("warning: camera (%s) contains scale factor, ignoring" % b_camera.name)

            # Blender's camera initially pointing (0, 0, -1) with up (0, 1, 0) in its math.py system
            # (also note that Blender's quaternion works this way, does not require q*v*q').
            cam_dir = rot @ mathutils.Vector((0, 0, -1))
            cam_up_dir = rot @ mathutils.Vector((0, 1, 0))
            fov_degrees = 70.0

            lens_unit = b_camera.lens_unit
            if lens_unit == "FOV":
                fov_degrees = math.degrees(b_camera.angle)
            elif lens_unit == "MILLIMETERS":
                sensor_width = b_camera.sensor_width
                focal_length = b_camera.lens
                fov_degrees = math.degrees(math.atan((sensor_width / 2.0) / focal_length)) * 2.0
            else:
                print("warning: camera (%s) with lens unit %s is unsupported, not exporting" % (
                    b_camera.name, b_camera.lens_unit))

            if not b_camera.ph_has_dof:
                position = utility.to_photon_vec3(pos)
                direction = utility.to_photon_vec3(cam_dir)
                up_direction = utility.to_photon_vec3(cam_up_dir)

                camera = PinholeReceiverCreator()
                camera.set_fov_degree(SDLReal(fov_degrees))
                camera.set_position(SDLVector3(position))
                camera.set_direction(SDLVector3(direction))
                camera.set_up_axis(SDLVector3(up_direction))

            else:
                position = utility.to_photon_vec3(pos)
                direction = utility.to_photon_vec3(cam_dir)
                up_direction = utility.to_photon_vec3(cam_up_dir)

                camera = ThinLensReceiverCreator()
                camera.set_fov_degree(SDLReal(fov_degrees))
                camera.set_position(SDLVector3(position))
                camera.set_direction(SDLVector3(direction))
                camera.set_up_axis(SDLVector3(up_direction))
                camera.set_lens_radius_mm(SDLReal(b_camera.ph_lens_radius_mm))
                camera.set_focal_distance_mm(SDLReal(b_camera.ph_focal_meters * 1000))

        if camera is not None:
            camera.set_data_name("receiver")
            camera.set_resolution_x(SDLInteger(resolution_x))
            camera.set_resolution_y(SDLInteger(resolution_y))
            self.__sdlconsole.queue_command(camera)
        else:
            print("warning: camera (%s) type (%s) is unsupported, not exporting" % (b_camera.name, b_camera.type))

    def export_world(self, b_world):
        actor_name = "ph_" + b_world.name

        creator = None
        if b_world.ph_background_type == 'IMAGE' and b_world.ph_image_file_path != "":
            creator = DomeActorCreator()
            creator.set_data_name(actor_name)
            creator.set_type(SDLString("image"))

            image_path = bpy.path.abspath(b_world.ph_image_file_path)
            image_sdlri = sdlresource.SdlResourceIdentifier()
            image_sdlri.append_folder(b_world.name + "_data")
            image_sdlri.set_file(utility.get_filename(image_path))
            creator.set_image(SDLString(image_sdlri.get_identifier()))

            # copy the envmap to scene folder
            self.get_sdlconsole().create_resource_folder(image_sdlri)
            dst_path = utility.get_appended_path(
                self.get_sdlconsole().get_working_directory(),
                image_sdlri.get_path())
            shutil.copyfile(image_path, dst_path)
        elif b_world.ph_background_type == 'PREETHAM':
            creator = DomeActorCreator()
            creator.set_data_name(actor_name)
            creator.set_type(SDLString("preetham"))

            creator.set_turbidity(SDLReal(b_world.ph_preetham_turbidity))
            creator.set_standard_time_24h(SDLReal(b_world.ph_standard_time))
            creator.set_standard_meridian_degrees(SDLReal(b_world.ph_standard_meridian))
            creator.set_site_latitude_decimal(SDLReal(b_world.ph_latitude))
            creator.set_site_longitude_decimal(SDLReal(b_world.ph_longitude))
            creator.set_julian_date(SDLReal(b_world.ph_julian_date))

        if creator is not None:
            self.get_sdlconsole().queue_command(creator)

            rotation = DomeActorRotate()
            rotation.set_target_name(actor_name)
            rotation.set_axis(SDLVector3((0, 1, 0)))
            rotation.set_degree(SDLReal(b_world.ph_up_rotation))
            self.get_sdlconsole().queue_command(rotation)

    def export_core_commands(self, b_scene):
        meta_info = meta.MetaGetter(b_scene)

        sample_generator = None
        if b_scene.ph_render_sample_generator_type == 'RANDOM':
            sample_generator = UniformRandomSampleGeneratorCreator()
            sample_generator.set_sample_amount(SDLInteger(meta_info.spp()))
        elif b_scene.ph_render_sample_generator_type == 'STRATIFIED':
            sample_generator = StratifiedSampleGeneratorCreator()
            sample_generator.set_sample_amount(SDLInteger(meta_info.spp()))
        elif b_scene.ph_render_sample_generator_type == 'HALTON':
            sample_generator = HaltonSampleGeneratorCreator()
            sample_generator.set_sample_amount(SDLInteger(meta_info.spp()))

        if sample_generator is not None:
            sample_generator.set_data_name("sample-generator")
            self.get_sdlconsole().queue_command(sample_generator)
        else:
            print("warning: no sample generator present")

        render_method = meta_info.render_method()

        renderer = None

        if render_method == "BVPT" or render_method == "BNEEPT" or render_method == "BVPTDL":
            renderer = EqualSamplingRendererCreator()
            renderer.set_filter_name(SDLString(meta_info.sample_filter_name()))
            renderer.set_estimator(SDLString(meta_info.integrator_type_name()))
            renderer.set_scheduler(SDLString(b_scene.ph_scheduler_type))
        elif render_method == "VPM":
            renderer = PmRendererCreator()
            renderer.set_mode(SDLString("vanilla"))
            renderer.set_num_photons(SDLInteger(b_scene.ph_render_num_photons))
            renderer.set_num_samples_per_pixel(SDLInteger(b_scene.ph_render_num_spp_pm))
            renderer.set_radius(SDLReal(b_scene.ph_render_kernel_radius))
        elif render_method == "PPM" or render_method == "SPPM":
            mode_name = "progressive" if render_method == "PPM" else "stochastic-progressive"
            renderer = PmRendererCreator()
            renderer.set_mode(SDLString(mode_name))
            renderer.set_num_photons(SDLInteger(b_scene.ph_render_num_photons))
            renderer.set_num_samples_per_pixel(SDLInteger(b_scene.ph_render_num_spp_pm))
            renderer.set_radius(SDLReal(b_scene.ph_render_kernel_radius))
            renderer.set_num_passes(SDLInteger(b_scene.ph_render_num_passes))
        elif render_method == "ATTRIBUTE":
            renderer = AttributeRendererCreator()
        elif render_method == "CUSTOM":
            custom_renderer_sdl_command = RawCommand()
            custom_renderer_sdl_command.append_string(b_scene.ph_render_custom_sdl)
            custom_renderer_sdl_command.append_string("\n")
            self.get_sdlconsole().queue_command(custom_renderer_sdl_command)
        else:
            print("warning: render method %s is not supported" % render_method)

        if renderer is not None:

            renderer.set_data_name("renderer")

            if b_scene.ph_use_crop_window:
                renderer.set_rect_x(SDLInteger(b_scene.ph_crop_min_x))
                renderer.set_rect_y(SDLInteger(b_scene.ph_crop_min_y))
                renderer.set_rect_w(SDLInteger(b_scene.ph_crop_width))
                renderer.set_rect_h(SDLInteger(b_scene.ph_crop_height))

            self.get_sdlconsole().queue_command(renderer)

    def export_options(self, b_scene):
        cook_settings = CookSettingsOptionCreator()
        cook_settings.set_data_name("cook-settings")
        if b_scene.ph_top_level_accelerator == 'BF':
            cook_settings.set_top_level_accelerator(SDLString("brute-force"))
        elif b_scene.ph_top_level_accelerator == 'BVH':
            cook_settings.set_top_level_accelerator(SDLString("bvh"))
        elif b_scene.ph_top_level_accelerator == 'IKD':
            cook_settings.set_top_level_accelerator(SDLString("indexed-kd-tree"))
        self.get_sdlconsole().queue_command(cook_settings)

        engine_option = EngineOptionCreator()
        engine_option.set_data_name("engine-option")
        engine_option.set_renderer(SDLString("@renderer"))# HACK
        engine_option.set_receiver(SDLString("@receiver"))# HACK
        engine_option.set_sample_generator(SDLString("@sample-generator"))# HACK
        engine_option.set_cook_settings(SDLString("@cook-settings"))# HACK
        self.get_sdlconsole().queue_command(engine_option)

    # TODO: write/flush commands to disk once a while (reducing memory usage)
    def export(self, b_depsgraph: bpy.types.Depsgraph):
        b_camera_object = scene.find_active_camera_object(b_depsgraph)
        b_mesh_objects = scene.find_mesh_objects(b_depsgraph)
        b_materials = scene.find_materials_from_mesh_objects(b_mesh_objects)
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

            # HACK: In Blender 2.8, materials from evaluated depsgraph will have all their properties left as default
            # values (Blender bug?), the original data block is good though
            # self.export_material(b_material)
            self.export_material(b_material.original)

        for b_mesh_object in b_mesh_objects:
            print("exporting mesh object: " + b_mesh_object.name)
            self.export_mesh_object(b_mesh_object)

        for b_light_object in b_light_objects:
            print("exporting light object: " + b_light_object.name)
            light.light_object_to_sdl_actor(b_light_object, self.get_sdlconsole())

        b_world = b_depsgraph.scene_eval.world
        self.export_world(b_world)
