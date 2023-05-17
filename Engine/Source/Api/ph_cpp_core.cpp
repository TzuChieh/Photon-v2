#include "ph_cpp_core.h"
#include "Api/init_and_exit.h"
#include "Common/logging.h"
#include "Common/Log/Logger.h"
#include "Common/config.h"

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

// Light Sources
#include "Actor/LightSource/LightSource.h"
#include "Actor/LightSource/ModelSource.h"
#include "Actor/LightSource/AreaSource.h"
#include "Actor/LightSource/PointSource.h"
#include "Actor/LightSource/SphereSource.h"
#include "Actor/LightSource/RectangleSource.h"
#include "Actor/LightSource/IesAttenuatedSource.h"

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
#include "Actor/Image/CheckerboardImage.h"
#include "Actor/Image/GradientImage.h"

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
#include "Actor/Light/ALight.h"
#include "Actor/ADome.h"
#include "Actor/APhantomModel.h"
#include "Actor/ATransformedInstance.h"
#include "Actor/Dome/AImageDome.h"
#include "Actor/Dome/APreethamDome.h"

// Enums
#include "EngineEnv/sdl_accelerator_type.h"
#include "EngineEnv/Visualizer/sdl_ray_energy_estimator_type.h"
#include "EngineEnv/Visualizer/sdl_sample_filter_type.h"
#include "EngineEnv/Visualizer/sdl_scheduler_type.h"
#include "Actor/Image/sdl_image_enums.h"
#include "Actor/SDLExtension/sdl_color_space_type.h"
#include "Actor/SDLExtension/sdl_color_usage_type.h"
#include "Actor/Material/Utility/EInterfaceFresnel.h"
#include "Actor/Material/Utility/EInterfaceMicrosurface.h"
#include "Actor/Material/Utility/ERoughnessToAlpha.h"

#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(CppAPI, Engine);

bool init_render_engine(EngineInitSettings settings)
{
	detail::core_logging::init();

	if(!settings.additionalLogHandlers.empty())
	{
		PH_LOG(CppAPI, "adding {} additional log handler(s)", settings.additionalLogHandlers.size());
		for(LogHandler& handler : settings.additionalLogHandlers)
		{
			if(!handler)
			{
				PH_LOG_WARNING(CppAPI,
					"attempting to add a null core log handler");
				continue;
			}
			
			detail::core_logging::get_core_logger().addLogHandler(std::move(handler));
		}

		settings.additionalLogHandlers.clear();
	}

	if(!init_engine_IO_infrastructure())
	{
		PH_LOG_ERROR(CppAPI, "IO infrastructure initialization failed");
		return false;
	}

	// Get SDL enums once here to initialize them--this is not required,
	// just to be safe as SDL enum instances are lazy-constructed and may
	// be done in strange places/order later (which may cause problems).
	// Enums are initialized first as they have less dependencies.
	//
	const std::vector<const SdlEnum*> sdlEnums = get_registered_engine_enums();
	PH_LOG(CppAPI, "initialized {} SDL enum definitions", sdlEnums.size());

	// Get SDL classes once here to initialize them--this is not required,
	// same reason as SDL enums.
	//
	const std::vector<const SdlClass*> sdlClasses = get_registered_engine_classes();
	PH_LOG(CppAPI, "initialized {} SDL class definitions", sdlClasses.size());

	return true;
}

bool exit_render_engine()
{
	if(!exit_API_database())
	{
		PH_LOG_ERROR(CppAPI, "C API database exiting failed");
		return false;
	}

	detail::core_logging::exit();

	return true;
}

namespace
{

template<typename SdlClassType>
const SdlClass* get_sdl_class()
{
	return SdlClassType::getSdlClass();
}

template<typename EnumType>
const SdlEnum* get_sdl_enum()
{
	return TSdlEnum<EnumType>::getSdlEnum();
}

}// end anonymous namespace

std::vector<const SdlClass*> get_registered_engine_classes()
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
		get_sdl_class<BinaryMixedSurfaceMaterial>(),

		// Light Sources
		get_sdl_class<LightSource>(),
		get_sdl_class<AreaSource>(),
		get_sdl_class<SphereSource>(),
		get_sdl_class<RectangleSource>(),
		get_sdl_class<PointSource>(),
		get_sdl_class<ModelSource>(),

		// Images
		get_sdl_class<Image>(),
		get_sdl_class<ConstantImage>(),
		get_sdl_class<RasterImageBase>(),
		get_sdl_class<RasterFileImage>(),
		get_sdl_class<MathImage>(),

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
		get_sdl_class<ALight>(),
		get_sdl_class<ADome>(),
		get_sdl_class<AImageDome>(),
		get_sdl_class<APreethamDome>(),
	};
}

std::vector<const SdlEnum*> get_registered_engine_enums()
{
	return
	{
		get_sdl_enum<EAccelerator>(),
		get_sdl_enum<ERayEnergyEstimator>(),
		get_sdl_enum<ESampleFilter>(),
		get_sdl_enum<EImageSampleMode>(),
		get_sdl_enum<EImageWrapMode>(),
		get_sdl_enum<math::EColorSpace>(),
		get_sdl_enum<math::EColorUsage>(),
		get_sdl_enum<EMathImageOp>(),
		get_sdl_enum<EInterfaceFresnel>(),
		get_sdl_enum<ERoughnessToAlpha>(),
		get_sdl_enum<EIdealSubstance>(),
		get_sdl_enum<ESurfaceMaterialMixMode>(),
	};
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
	return Path(PH_RESOURCE_DIRECTORY).append(to_string(project));
}

}// end namespace ph
