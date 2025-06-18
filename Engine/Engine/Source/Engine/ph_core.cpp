#include "Engine/ph_core.h"
#include "Engine/init_and_exit.h"
#include "Engine/DataIO/FileSystem/Path.h"

// Geometries
#include "Engine/Actor/Geometry/Geometry.h"
#include "Engine/Actor/Geometry/GSphere.h"
#include "Engine/Actor/Geometry/GRectangle.h"
#include "Engine/Actor/Geometry/GTriangle.h"
#include "Engine/Actor/Geometry/GTriangleMesh.h"
#include "Engine/Actor/Geometry/GMengerSponge.h"
#include "Engine/Actor/Geometry/GCuboid.h"
#include "Engine/Actor/Geometry/GEmpty.h"
#include "Engine/Actor/Geometry/GeometrySoup.h"
#include "Engine/Actor/Geometry/GPlyPolygonMesh.h"

// Materials
#include "Engine/Actor/Material/Material.h"
#include "Engine/Actor/Material/SurfaceMaterial.h"
#include "Engine/Actor/Material/VolumeMaterial.h"
#include "Engine/Actor/Material/MatteOpaque.h"
#include "Engine/Actor/Material/AbradedOpaque.h"
#include "Engine/Actor/Material/AbradedTranslucent.h"
#include "Engine/Actor/Material/IdealSubstance.h"
#include "Engine/Actor/Material/BinaryMixedSurfaceMaterial.h"
#include "Engine/Actor/Material/FullMaterial.h"
#include "Engine/Actor/Material/Volume/IdealMedium.h"
#include "Engine/Actor/Material/LayeredSurface.h"
#include "Engine/Actor/Material/ThinFilm.h"
#include "Engine/Actor/Material/ThinDielectricSurface.h"
#include "Engine/Actor/Material/SurfaceNormalMap.h"

// Motions
#include "Engine/Actor/MotionSource/MotionSource.h"
#include "Engine/Actor/MotionSource/ConstantVelocityMotion.h"

// Images
#include "Engine/Actor/Image/Image.h"
#include "Engine/Actor/Image/ConstantImage.h"
#include "Engine/Actor/Image/RasterImageBase.h"
#include "Engine/Actor/Image/RasterFileImage.h"
#include "Engine/Actor/Image/ConstantImage.h"
#include "Engine/Actor/Image/MathImage.h"
#include "Engine/Actor/Image/SwizzledImage.h"
#include "Engine/Actor/Image/CheckerboardImage.h"
#include "Engine/Actor/Image/GradientImage.h"
#include "Engine/Actor/Image/BlackBodyRadiationImage.h"

// Observers
#include "Engine/EngineEnv/Observer/Observer.h"
#include "Engine/EngineEnv/Observer/OrientedRasterObserver.h"
#include "Engine/EngineEnv/Observer/SingleLensObserver.h"

// Sample Sources
#include "Engine/EngineEnv/SampleSource/SampleSource.h"
#include "Engine/EngineEnv/SampleSource/RuntimeSampleSource.h"
#include "Engine/EngineEnv/SampleSource/UniformRandomSampleSource.h"
#include "Engine/EngineEnv/SampleSource/StratifiedSampleSource.h"
#include "Engine/EngineEnv/SampleSource/HaltonSampleSource.h"

// Visualizers
#include "Engine/EngineEnv/Visualizer/Visualizer.h"
#include "Engine/EngineEnv/Visualizer/FrameVisualizer.h"
#include "Engine/EngineEnv/Visualizer/PathTracingVisualizer.h"
#include "Engine/EngineEnv/Visualizer/PhotonMappingVisualizer.h"

// Options
#include "Engine/SDL/Option.h"
#include "Engine/EngineEnv/Session/RenderSession.h"
#include "Engine/EngineEnv/Session/SingleFrameRenderSession.h"

// Objects
#include "Engine/SDL/Object.h"

// Actors
#include "Engine/Actor/Actor.h"
#include "Engine/Actor/PhysicalActor.h"
#include "Engine/Actor/AModel.h"
#include "Engine/Actor/AMaskedModel.h"
#include "Engine/Actor/ADome.h"
#include "Engine/Actor/APhantomModel.h"
#include "Engine/Actor/ATransformedInstance.h"
#include "Engine/Actor/Dome/AImageDome.h"
#include "Engine/Actor/Dome/APreethamDome.h"
#include "Engine/Actor/Light/ALight.h"
#include "Engine/Actor/Light/AGeometricLight.h"
#include "Engine/Actor/Light/AAreaLight.h"
#include "Engine/Actor/Light/AModelLight.h"
#include "Engine/Actor/Light/APointLight.h"
#include "Engine/Actor/Light/ARectangleLight.h"
#include "Engine/Actor/Light/ASphereLight.h"
#include "Engine/Actor/Light/AIesAttenuatedLight.h"

// Enums
#include "Engine/EngineEnv/SampleSource/sdl_halton_randomization_types.h"
#include "Engine/EngineEnv/sdl_accelerator_type.h"
#include "Engine/EngineEnv/Visualizer/sdl_visualizer_types.h"
#include "Engine/Actor/Image/sdl_image_enums.h"
#include "Engine/Actor/SDLExtension/sdl_color_enums.h"
#include "Engine/Actor/Material/Component/sdl_component_enums.h"
#include "Engine/DataIO/sdl_picture_file_type.h"

#include <Common/config.h>
#include <Common/logging.h>
#include <Common/Log/Logger.h>

#include <utility>
#include <vector>
#include <string>

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
		get_sdl_class<GCuboid>(),
		get_sdl_class<GMengerSponge>(),
		get_sdl_class<GeometrySoup>(),
		get_sdl_class<GPlyPolygonMesh>(),

		// Materials
		get_sdl_class<Material>(),
		get_sdl_class<SurfaceMaterial>(),
		get_sdl_class<VolumeMaterial>(),
		get_sdl_class<MatteOpaque>(),
		get_sdl_class<AbradedOpaque>(),
		get_sdl_class<AbradedTranslucent>(),
		get_sdl_class<FullMaterial>(),
		get_sdl_class<IdealSubstance>(),
		get_sdl_class<LayeredSurface>(),
		get_sdl_class<BinaryMixedSurfaceMaterial>(),
		get_sdl_class<ThinDielectricSurface>(),
		get_sdl_class<IdealMedium>(),
		get_sdl_class<SurfaceNormalMap>(),

		// Images
		get_sdl_class<Image>(),
		get_sdl_class<ConstantImage>(),
		get_sdl_class<RasterImageBase>(),
		get_sdl_class<RasterFileImage>(),
		get_sdl_class<MathImage>(),
		get_sdl_class<SwizzledImage>(),
		get_sdl_class<CheckerboardImage>(),
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
		get_sdl_class<AMaskedModel>(),
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
		get_sdl_enum<EMaskingShadowing>(),
		get_sdl_enum<EIdealSubstance>(),
		get_sdl_enum<ESurfaceMaterialMixMode>(),
		get_sdl_enum<EPictureFile>(),
	};
}

}// end anonymous namespace

bool init_render_engine(std::optional<EngineInitSettings> settings)
{
	detail::core_logging::init();

	if(!settings)
	{
		settings = EngineInitSettings::loadStandardConfig();
	}

	if(!settings->additionalLogHandlers.empty())
	{
		PH_LOG(CppAPI, Note, "adding {} additional log handler(s)", settings->additionalLogHandlers.size());
		for(LogHandler& handler : settings->additionalLogHandlers)
		{
			if(!handler)
			{
				PH_LOG(CppAPI, Warning,
					"attempting to add a null core log handler");
				continue;
			}
			
			detail::core_logging::get_logger().addLogHandler(std::move(handler));
		}

		settings->additionalLogHandlers.clear();
	}

	if(!init_engine_core(*settings))
	{
		PH_LOG(CppAPI, Error, "core initialization failed");
		return false;
	}

	if(!init_engine_IO_infrastructure(*settings))
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

	after_engine_init(*settings);

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
