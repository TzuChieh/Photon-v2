# ========================================
# NOTE: THIS FILE CONTAINS GENERATED CODE 
#       DO NOT MODIFY                     
# ========================================
# last generated: 00:20:07 2021-09-03 

from abc import ABC, abstractmethod


class SDLData(ABC):
    def __init__(self):
        super().__init__()

    @abstractmethod
    def get_type(self):
        pass

    @abstractmethod
    def generate_data(self):
        pass


class SDLString(SDLData):
    def __init__(self, string=""):
        super().__init__()
        self.string = string

    def get_type(self):
        return "string"

    def generate_data(self):
        return self.string


class SDLInteger(SDLData):
    def __init__(self, integer=0):
        super().__init__()
        self.integer = integer

    def get_type(self):
        return "integer"

    def generate_data(self):
        return str(self.integer)


class SDLReal(SDLData):
    def __init__(self, real=0):
        super().__init__()
        self.real = real

    def get_type(self):
        return "real"

    def generate_data(self):
        return str(self.real)


class SDLVector3(SDLData):
    def __init__(self, vector=(0, 0, 0)):
        super().__init__()
        self.vector = vector

    def get_type(self):
        return "vector3"

    def generate_data(self):
        return "\"%f %f %f\"" % (self.vector[0], self.vector[1], self.vector[2])


class SDLQuaternion(SDLData):
    def __init__(self, quaternion=(0, 0, 0, 0)):
        super().__init__()
        self.quaternion = quaternion

    def get_type(self):
        return "quaternion"

    def generate_data(self):
        return "\"%f %f %f %f\"" % (self.quaternion[0], self.quaternion[1], self.quaternion[2], self.quaternion[3])


class SDLRealArray(SDLData):
    def __init__(self, array=None):
        super().__init__()
        if array is None:
            self.array = []
        else:
            self.array = array

    def get_type(self):
        return "real-array"

    def generate_data(self):
        fragments = ["{"]
        for real in self.array:
            fragments.append("%f " % real)
        fragments.append("}")
        return "".join(fragments)

    def set_array(self, array):
        self.array = array
        return self

    def add(self, real):
        self.array.append(real)
        return self


class SDLVector3Array(SDLData):
    def __init__(self, array=None):
        super().__init__()
        if array is None:
            self.array = []
        else:
            self.array = array

    def get_type(self):
        return "vector3-array"

    def generate_data(self):
        fragments = ["{"]
        for vector3 in self.array:
            fragments.append("\"%f %f %f\" " % (vector3[0], vector3[1], vector3[2]))
        fragments.append("}")
        return "".join(fragments)

    def set_array(self, array):
        self.array = array
        return self

    def add(self, vector3):
        self.array.append(vector3)
        return self


class SDLReference(SDLData):
    def __init__(self, ref_type="", ref_name=""):
        super().__init__()
        self.ref_type = ref_type
        self.ref_name = ref_name

    def get_type(self):
        return self.ref_type

    def generate_data(self):
        return "\"@%s\"" % self.ref_name


class SDLRaw(SDLData):
    def __init__(self, type_string="", data_string=""):
        super().__init__()
        self.type_string = type_string
        self.data_string = data_string

    def get_type(self):
        return self.type_string

    def generate_data(self):
        return self.data_string


class SDLCommand(ABC):
    def __init__(self):
        super().__init__()
        self._inputs = []

    @abstractmethod
    def get_prefix(self):
        pass

    @abstractmethod
    def get_full_type(self):
        pass

    @abstractmethod
    def generate(self):
        pass

    def set_input(self, name, data: SDLData):
        self._inputs.append((name, data))

    def _generate_input_fragments(self, out_fragments):
        for name, data in self._inputs:
            out_fragments.append("[")
            out_fragments.append(data.get_type() + " ")
            out_fragments.append(name + " ")
            out_fragments.append(data.generate_data())
            out_fragments.append("]")


class SDLCreatorCommand(SDLCommand):
    def __init__(self):
        super().__init__()
        self.__data_name = ""

    @abstractmethod
    def get_full_type(self):
        pass

    def get_prefix(self):
        return "->"

    def generate(self):
        # TODO: some part can be pre-generated
        fragments = [
            self.get_prefix(), " ",
            self.get_full_type(), " ",
            "\"@" + self.__data_name + "\"", " "]
        self._generate_input_fragments(fragments)
        fragments.append("\n")

        return "".join(fragments)

    def set_data_name(self, data_name):
        self.__data_name = data_name


class SDLExecutorCommand(SDLCommand):
    def __init__(self):
        super().__init__()
        self.__target_name = ""

    @abstractmethod
    def get_full_type(self):
        pass

    @abstractmethod
    def get_name(self):
        pass

    def get_prefix(self):
        return "->"

    def generate(self):
        # TODO: some part can be pre-generated
        fragments = [
            self.get_prefix(), " ",
            self.get_full_type(), " ",
            self.get_name(), "(",
            "\"@" + self.__target_name + "\")", " "]
        self._generate_input_fragments(fragments)
        fragments.append("\n")

        return "".join(fragments)

    def set_target_name(self, data_name):
        self.__target_name = data_name


class SDLUnspecified(SDLReference):
    def __init__(self, ref_name = ""):
        super().__init__("unspecified", ref_name)


class SDLGeometry(SDLReference):
    def __init__(self, ref_name = ""):
        super().__init__("geometry", ref_name)


class SDLMaterial(SDLReference):
    def __init__(self, ref_name = ""):
        super().__init__("material", ref_name)


class SDLMotion(SDLReference):
    def __init__(self, ref_name = ""):
        super().__init__("motion", ref_name)


class SDLLightSource(SDLReference):
    def __init__(self, ref_name = ""):
        super().__init__("light-source", ref_name)


class SDLActor(SDLReference):
    def __init__(self, ref_name = ""):
        super().__init__("actor", ref_name)


class SDLImage(SDLReference):
    def __init__(self, ref_name = ""):
        super().__init__("image", ref_name)


class SDLFrameProcessor(SDLReference):
    def __init__(self, ref_name = ""):
        super().__init__("frame-processor", ref_name)


class SDLObserver(SDLReference):
    def __init__(self, ref_name = ""):
        super().__init__("observer", ref_name)


class SDLSampleSource(SDLReference):
    def __init__(self, ref_name = ""):
        super().__init__("sample-source", ref_name)


class SDLVisualizer(SDLReference):
    def __init__(self, ref_name = ""):
        super().__init__("visualizer", ref_name)


class SDLOption(SDLReference):
    def __init__(self, ref_name = ""):
        super().__init__("option", ref_name)




class SphereGeometryCreator(SDLCreatorCommand):
    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "geometry(sphere)"

    def set_radius(self, radius: SDLData):
        self.set_input("radius", radius)


class RectangleGeometryCreator(SDLCreatorCommand):
    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "geometry(rectangle)"

    def set_width(self, width: SDLData):
        self.set_input("width", width)

    def set_height(self, height: SDLData):
        self.set_input("height", height)

    def set_texcoord_scale(self, texcoord_scale: SDLData):
        self.set_input("texcoord-scale", texcoord_scale)


class MatteOpaqueMaterialCreator(SDLCreatorCommand):
    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "material(matte-opaque)"

    def set_albedo(self, albedo: SDLData):
        self.set_input("albedo", albedo)

    def set_sigma_degrees(self, sigma_degrees: SDLData):
        self.set_input("sigma-degrees", sigma_degrees)


class AbradedOpaqueMaterialCreator(SDLCreatorCommand):
    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "material(abraded-opaque)"

    def set_fresnel(self, fresnel: SDLData):
        self.set_input("fresnel", fresnel)

    def set_f0(self, f0: SDLData):
        self.set_input("f0", f0)

    def set_ior_outer(self, ior_outer: SDLData):
        self.set_input("ior-outer", ior_outer)

    def set_ior_inner_n(self, ior_inner_n: SDLData):
        self.set_input("ior-inner-n", ior_inner_n)

    def set_ior_inner_k(self, ior_inner_k: SDLData):
        self.set_input("ior-inner-k", ior_inner_k)

    def set_microsurface(self, microsurface: SDLData):
        self.set_input("microsurface", microsurface)

    def set_roughness_to_alpha(self, roughness_to_alpha: SDLData):
        self.set_input("roughness-to-alpha", roughness_to_alpha)

    def set_roughness(self, roughness: SDLData):
        self.set_input("roughness", roughness)

    def set_roughness_v(self, roughness_v: SDLData):
        self.set_input("roughness-v", roughness_v)


class FullMaterialCreator(SDLCreatorCommand):
    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "material(full)"

    def set_surface(self, surface: SDLData):
        self.set_input("surface", surface)

    def set_interior(self, interior: SDLData):
        self.set_input("interior", interior)

    def set_exterior(self, exterior: SDLData):
        self.set_input("exterior", exterior)


class SphereLightSourceCreator(SDLCreatorCommand):
    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "light-source(sphere)"

    def set_radius(self, radius: SDLData):
        self.set_input("radius", radius)


class RectangleLightSourceCreator(SDLCreatorCommand):
    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "light-source(rectangle)"

    def set_width(self, width: SDLData):
        self.set_input("width", width)

    def set_height(self, height: SDLData):
        self.set_input("height", height)


class SingleLensObserverCreator(SDLCreatorCommand):
    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "observer(single-lens)"

    def set_lens_radius_mm(self, lens_radius_mm: SDLData):
        self.set_input("lens-radius-mm", lens_radius_mm)

    def set_focal_distance_mm(self, focal_distance_mm: SDLData):
        self.set_input("focal-distance-mm", focal_distance_mm)

    def set_sensor_width_mm(self, sensor_width_mm: SDLData):
        self.set_input("sensor-width-mm", sensor_width_mm)

    def set_sensor_offset_mm(self, sensor_offset_mm: SDLData):
        self.set_input("sensor-offset-mm", sensor_offset_mm)

    def set_fov_degrees(self, fov_degrees: SDLData):
        self.set_input("fov-degrees", fov_degrees)


class UniformRandomSampleSourceCreator(SDLCreatorCommand):
    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "sample-source(uniform-random)"


class StratifiedSampleSourceCreator(SDLCreatorCommand):
    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "sample-source(stratified)"


class HaltonSampleSourceCreator(SDLCreatorCommand):
    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "sample-source(halton)"


class PathTracingVisualizerCreator(SDLCreatorCommand):
    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "visualizer(path-tracing)"

    def set_scheduler(self, scheduler: SDLData):
        self.set_input("scheduler", scheduler)

    def set_estimator(self, estimator: SDLData):
        self.set_input("estimator", estimator)

    def set_sample_filter(self, sample_filter: SDLData):
        self.set_input("sample-filter", sample_filter)


class OptionOptionCreator(SDLCreatorCommand):
    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "option(option)"


class SingleFrameRenderSessionOptionCreator(SDLCreatorCommand):
    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "option(single-frame-render-session)"

    def set_frame_size(self, frame_size: SDLData):
        self.set_input("frame-size", frame_size)

    def set_visualizer(self, visualizer: SDLData):
        self.set_input("visualizer", visualizer)

    def set_observer(self, observer: SDLData):
        self.set_input("observer", observer)

    def set_sample_source(self, sample_source: SDLData):
        self.set_input("sample-source", sample_source)

    def set_top_level_accelerator(self, top_level_accelerator: SDLData):
        self.set_input("top-level-accelerator", top_level_accelerator)


class PhysicalActorTranslate(SDLExecutorCommand):
    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "actor(physical)"

    def get_name(self):
        return "translate"

    def set_amount(self, amount: SDLData):
        self.set_input("amount", amount)


class PhysicalActorRotate(SDLExecutorCommand):
    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "actor(physical)"

    def get_name(self):
        return "rotate"

    def set_axis(self, axis: SDLData):
        self.set_input("axis", axis)

    def set_degrees(self, degrees: SDLData):
        self.set_input("degrees", degrees)

    def set_rotation(self, rotation: SDLData):
        self.set_input("rotation", rotation)


class PhysicalActorScale(SDLExecutorCommand):
    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "actor(physical)"

    def get_name(self):
        return "scale"

    def set_amount(self, amount: SDLData):
        self.set_input("amount", amount)


class ModelActorCreator(SDLCreatorCommand):
    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "actor(model)"

    def set_geometry(self, geometry: SDLData):
        self.set_input("geometry", geometry)

    def set_material(self, material: SDLData):
        self.set_input("material", material)

    def set_motion(self, motion: SDLData):
        self.set_input("motion", motion)


class LightActorCreator(SDLCreatorCommand):
    def __init__(self):
        super().__init__()

    def get_full_type(self):
        return "actor(light)"

    def set_source(self, source: SDLData):
        self.set_input("source", source)


