#include "ph_core.h"
#include "Api/init_and_exit.h"
#include "DataIO/FileSystem/Path.h"

// Geometries
#include "Actor/Geometry/Geometry.h"
#include "Actor/Geometry/GSphere.h"
#include "Actor/Geometry/GRectangle.h"
#include "Actor/Geometry/GTriangle.h"
#include "Actor/Geometry/GTriangleMesh.h"
#include "Actor/Geometry/GMengerSponge.h"
#include "Actor/Geometry/GCuboid.h"
#include "Actor/Geometry/GEmpty.h"
#include "Actor/Geometry/GeometrySoup.h"
#include "Actor/Geometry/GPlyPolygonMesh.h"

// Materials
#include "Actor/Material/Material.h"
#include "Actor/Material/MatteOpaque.h"
#include "Actor/Material/AbradedOpaque.h"
#include "Actor/Material/AbradedTranslucent.h"
#include "Actor/Material/IdealSubstance.h"
#include "Actor/Material/BinaryMixedSurfaceMaterial.h"
#include "Actor/Material/FullMaterial.h"
#include "Actor/Material/Volume/VAbsorptionOnly.h"
#include "Actor/Material/LayeredSurface.h"
#include "Actor/Material/ThinFilm.h"

// Motions
#include "Actor/MotionSource/MotionSource.h"
#include "Actor/MotionSource/ConstantVelocityMotion.h"

// Images
#include "Actor/Image/Image.h"
#include "Actor/Image/ConstantImage.h"
#include "Actor/Image/RasterImageBase.h"
#include "Actor/Image/RasterFileImage.h"
#include "Actor/Image/ConstantImage.h"
#include "Actor/Image/MathImage.h"
#include "Actor/Image/SwizzledImage.h"
#include "Actor/Image/CheckerboardImage.h"
#include "Actor/Image/GradientImage.h"
#include "Actor/Image/BlackBodyRadiationImage.h"

// Observers
#include "EngineEnv/Observer/Observer.h"
#include "EngineEnv/Observer/OrientedRasterObserver.h"
#include "EngineEnv/Observer/SingleLensObserver.h"

// Sample Sources
#include "EngineEnv/SampleSource/SampleSource.h"
#include "EngineEnv/SampleSource/RuntimeSampleSource.h"
#include "EngineEnv/SampleSource/UniformRandomSampleSource.h"
#include "EngineEnv/SampleSource/StratifiedSampleSource.h"
#include "EngineEnv/SampleSource/HaltonSampleSource.h"

// Visualizers
#include "EngineEnv/Visualizer/Visualizer.h"
#include "EngineEnv/Visualizer/FrameVisualizer.h"
#include "EngineEnv/Visualizer/PathTracingVisualizer.h"
#include "EngineEnv/Visualizer/PhotonMappingVisualizer.h"

// Options
#include "SDL/Option.h"
#include "EngineEnv/Session/RenderSession.h"
#include "EngineEnv/Session/SingleFrameRenderSession.h"

// Objects
#include "SDL/Object.h"

// Actors
#include "Actor/Actor.h"
#include "Actor/PhysicalActor.h"
#include "Actor/AModel.h"
#include "Actor/ADome.h"
#include "Actor/APhantomModel.h"
#include "Actor/ATransformedInstance.h"
#include "Actor/Dome/AImageDome.h"
#include "Actor/Dome/APreethamDome.h"
#include "Actor/Light/ALight.h"
#include "Actor/Light/AGeometricLight.h"
#include "Actor/Light/AAreaLight.h"
#include "Actor/Light/AModelLight.h"
#include "Actor/Light/APointLight.h"
#include "Actor/Light/ARectangleLight.h"
#include "Actor/Light/ASphereLight.h"
#include "Actor/Light/AIesAttenuatedLight.h"

// Enums
#include "EngineEnv/SampleSource/sdl_halton_randomization_types.h"
#include "EngineEnv/sdl_accelerator_type.h"
#include "EngineEnv/Visualizer/sdl_ray_energy_estimator_type.h"
#include "EngineEnv/Visualizer/sdl_sample_filter_type.h"
#include "EngineEnv/Visualizer/sdl_scheduler_type.h"
#include "EngineEnv/Visualizer/sdl_photon_mapping_mode.h"
#include "Actor/Image/sdl_image_enums.h"
#include "Actor/SDLExtension/sdl_color_enums.h"
#include "Actor/Material/Component/EInterfaceFresnel.h"
#include "Actor/Material/Component/EInterfaceMicrosurface.h"
#include "Actor/Material/Component/ERoughnessToAlpha.h"
#include "DataIO/sdl_picture_file_type.h"

#include <Common/config.h>
#include <Common/logging.h>
#include <Common/Log/Logger.h>

#include <utility>
#include <vector>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(CppAPI, Engine);

namespace
{

template<typename SdlClassType>
inline const SdlClass* get_sdl_class()
{
	return SdlClassType::getSdlClass();
}

template<typename EnumType>
inline const SdlEnum* get_sdl_enum()
{
	return TSdlEnum<EnumType>::getSdlEnum();
}

/* The following section registeres SDL classes and enums to the engine. Please note that SDL
interface definition and reflection do not need registration to work, this simply provide an
interface to available classes and enums so some functionalities can benefit from it.
*/

inline std::vector<const SdlClass*> register_engine_classes()
{
	return
	{
		// Geometries
		get_sdl_class<Geometry>(),
		get_sdl_class<GSphere>(),
		get_sdl_class<GRectangle>(),
		get_sdl_class<GTriangle>(),
		get_sdl_class<GTriangleMesh>(),
		//get_sdl_class<GCuboid>(),
		get_sdl_class<GMengerSponge>(),
		get_sdl_class<GeometrySoup>(),
		get_sdl_class<GPlyPolygonMesh>(),

		// Materials
		get_sdl_class<Material>(),
		get_sdl_class<SurfaceMaterial>(),
		get_sdl_class<MatteOpaque>(),
		get_sdl_class<AbradedOpaque>(),
		get_sdl_class<AbradedTranslucent>(),
		get_sdl_class<FullMaterial>(),
		get_sdl_class<IdealSubstance>(),
		get_sdl_class<LayeredSurface>(),
		get_sdl_class<BinaryMixedSurfaceMaterial>(),

		// Images
		get_sdl_class<Image>(),
		get_sdl_class<ConstantImage>(),
		get_sdl_class<RasterImageBase>(),
		get_sdl_class<RasterFileImage>(),
		get_sdl_class<MathImage>(),
		get_sdl_class<SwizzledImage>(),
		get_sdl_class<BlackBodyRadiationImage>(),

		// Observers
		get_sdl_class<Observer>(),
		get_sdl_class<OrientedRasterObserver>(),
		get_sdl_class<SingleLensObserver>(),

		// Sample Sources
		get_sdl_class<SampleSource>(),
		get_sdl_class<RuntimeSampleSource>(),
		get_sdl_class<UniformRandomSampleSource>(),
		get_sdl_class<StratifiedSampleSource>(),
		get_sdl_class<HaltonSampleSource>(),

		// Visualizers
		get_sdl_class<Visualizer>(),
		get_sdl_class<FrameVisualizer>(),
		get_sdl_class<PathTracingVisualizer>(),
		get_sdl_class<PhotonMappingVisualizer>(),

		// Options
		get_sdl_class<Option>(),
		get_sdl_class<RenderSession>(),
		get_sdl_class<SingleFrameRenderSession>(),

		// Objects
		get_sdl_class<Object>(),

		// Actors
		get_sdl_class<Actor>(),
		get_sdl_class<PhysicalActor>(),
		get_sdl_class<AModel>(),
		get_sdl_class<ADome>(),
		get_sdl_class<AImageDome>(),
		get_sdl_class<APreethamDome>(),
		get_sdl_class<ALight>(),
		get_sdl_class<AGeometricLight>(),
		get_sdl_class<AAreaLight>(),
		get_sdl_class<AModelLight>(),
		get_sdl_class<APointLight>(),
		get_sdl_class<ARectangleLight>(),
		get_sdl_class<ASphereLight>(),
		get_sdl_class<AIesAttenuatedLight>(),
	};
}

inline std::vector<const SdlEnum*> register_engine_enums()
{
	return
	{
		get_sdl_enum<EHaltonPermutation>(),
		get_sdl_enum<EHaltonSequence>(),
		get_sdl_enum<EAccelerator>(),
		get_sdl_enum<ERayEnergyEstimator>(),
		get_sdl_enum<ESampleFilter>(),
		get_sdl_enum<EScheduler>(),
		get_sdl_enum<EPhotonMappingMode>(),
		get_sdl_enum<EImageSampleMode>(),
		get_sdl_enum<EImageWrapMode>(),
		get_sdl_enum<math::EColorSpace>(),
		get_sdl_enum<math::EColorUsage>(),
		get_sdl_enum<EMathImageOp>(),
		get_sdl_enum<EInterfaceFresnel>(),
		get_sdl_enum<ERoughnessToAlpha>(),
		get_sdl_enum<EIdealSubstance>(),
		get_sdl_enum<ESurfaceMaterialMixMode>(),
		get_sdl_enum<EPictureFile>(),
	};
}

}// end anonymous namespace

bool init_render_engine(EngineInitSettings settings)
{
	detail::core_logging::init();

	if(!settings.additionalLogHandlers.empty())
	{
		PH_LOG(CppAPI, Note, "adding {} additional log handler(s)", settings.additionalLogHandlers.size());
		for(LogHandler& handler : settings.additionalLogHandlers)
		{
			if(!handler)
			{
				PH_LOG(CppAPI, Warning,
					"attempting to add a null core log handler");
				continue;
			}
			
			detail::core_logging::get_logger().addLogHandler(std::move(handler));
		}

		settings.additionalLogHandlers.clear();
	}

	if(!init_engine_core(settings))
	{
		PH_LOG(CppAPI, Error, "core initialization failed");
		return false;
	}

	if(!init_engine_IO_infrastructure(settings))
	{
		PH_LOG(CppAPI, Error, "IO infrastructure initialization failed");
		return false;
	}

	// Get SDL enums once here to initialize them--this is not required, just to be safe 
	// as SDL enum instances are lazy-constructed and may be done in strange places/order 
	// later (which may cause problems). Also, there may be some extra code in the definition
	// that want to be ran early.
	// Enums are initialized first as they have fewer dependencies.
	//
	const auto sdlEnums = get_registered_engine_enums();
	PH_DEBUG_LOG(CppAPI, "initialized {} SDL enum definitions", sdlEnums.size());

	// Get SDL classes once here to initialize them--this is not required,
	// same reason as SDL enums.
	//
	const auto sdlClasses = get_registered_engine_classes();
	PH_DEBUG_LOG(CppAPI, "initialized {} SDL class definitions", sdlClasses.size());

	after_engine_init(settings);

	return true;
}

bool exit_render_engine()
{
	before_engine_exit();

	if(!exit_API_database())
	{
		PH_LOG(CppAPI, Error, "C API database exiting failed");
		return false;
	}

	detail::core_logging::exit();

	return true;
}

std::span<const SdlClass* const> get_registered_engine_classes()
{
	static std::vector<const SdlClass*> classes = register_engine_classes();
	return classes;
}

std::span<const SdlEnum* const> get_registered_engine_enums()
{
	static std::vector<const SdlEnum*> enums = register_engine_enums();
	return enums;
}

Path get_config_directory(const EEngineProject project)
{
	return Path(PH_CONFIG_DIRECTORY).append(to_string(project));
}

Path get_internal_resource_directory(const EEngineProject project)
{
	return Path(PH_INTERNAL_RESOURCE_DIRECTORY).append(to_string(project));
}

Path get_resource_directory(const EEngineProject project)
{
	return Path(PH_RENDERER_RESOURCE_DIRECTORY).append(to_string(project));
}

Path get_script_directory(const EEngineProject project)
{
	return Path(PH_SCRIPT_DIRECTORY).append(to_string(project));
}

}// end namespace ph
