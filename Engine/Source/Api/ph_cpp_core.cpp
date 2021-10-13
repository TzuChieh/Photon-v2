#include "ph_cpp_core.h"
#include "Api/init_and_exit.h"
#include "Common/logging.h"
#include "Common/config.h"

// Geometries
#include "Actor/Geometry/Geometry.h"
#include "Actor/Geometry/GSphere.h"
#include "Actor/Geometry/GRectangle.h"
#include "Actor/Geometry/GTriangleMesh.h"
#include "Actor/Geometry/GMengerSponge.h"
#include "Actor/Geometry/GCuboid.h"
#include "Actor/Geometry/GEmpty.h"
#include "Actor/Geometry/GeometrySoup.h"

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
#include "Actor/Image/RasterImageBase.h"
#include "Actor/Image/RasterFileImage.h"

#include "Actor/Image/ConstantImage.h"
#include "Actor/Image/RealMathImage.h"
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
#include "DataIO/Option.h"
#include "EngineEnv/Session/RenderSession.h"
#include "EngineEnv/Session/SingleFrameRenderSession.h"

// Actors
#include "Actor/Actor.h"
#include "Actor/PhysicalActor.h"
#include "Actor/AModel.h"
#include "Actor/ALight.h"
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
#include "Actor/SDLExtension/sdl_color_space_type.h"
#include "Actor/SDLExtension/sdl_color_usage_type.h"

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(CppAPI, Engine);

bool init_render_engine()
{
	if(!init_core_infrastructure())
	{
		PH_LOG_ERROR(CppAPI, "core infrastructure initialization failed");
		return false;
	}

	// Get SDL enums once here to initialize them--this is not required,
	// just to be safe as SDL enum instances are lazy-constructed and may
	// be done in strange places/order later (which may cause problems).
	// Enums are initialized first as they have less dependencies.
	//
	const std::vector<const SdlEnum*> sdlEnums = get_registered_sdl_enums();
	PH_LOG(CppAPI, "initialized {} SDL enum definitions", sdlEnums.size());

	// Get SDL classes once here to initialize them--this is not required,
	// same reason as SDL enums.
	//
	const std::vector<const SdlClass*> sdlClasses = get_registered_sdl_classes();
	PH_LOG(CppAPI, "initialized {} SDL class definitions", sdlClasses.size());

	if(!init_command_parser())
	{
		PH_LOG_ERROR(CppAPI, "command parser initialization failed");
		return false;
	}

	return true;
}

bool exit_render_engine()
{
	if(!exit_api_database())
	{
		PH_LOG_ERROR(CppAPI, "C API database exiting failed");
		return false;
	}

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

}

std::vector<const SdlClass*> get_registered_sdl_classes()
{
	return
	{
		// Geometries
		get_sdl_class<Geometry>(),
		get_sdl_class<GSphere>(),
		get_sdl_class<GRectangle>(),
		//get_sdl_class<GCuboid>(),
		get_sdl_class<GMengerSponge>(),

		// Materials
		get_sdl_class<Material>(),
		get_sdl_class<SurfaceMaterial>(),
		get_sdl_class<MatteOpaque>(),
		get_sdl_class<AbradedOpaque>(),
		get_sdl_class<AbradedTranslucent>(),
		get_sdl_class<FullMaterial>(),

		// Light Sources
		get_sdl_class<LightSource>(),
		get_sdl_class<AreaSource>(),
		get_sdl_class<SphereSource>(),
		get_sdl_class<RectangleSource>(),
		get_sdl_class<PointSource>(),

		// Images
		get_sdl_class<Image>(),
		get_sdl_class<RasterImageBase>(),

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

		// Actors
		get_sdl_class<Actor>(),
		get_sdl_class<PhysicalActor>(),
		get_sdl_class<AModel>(),
		get_sdl_class<ALight>(),
		get_sdl_class<ADome>(),
	};
}

std::vector<const SdlEnum*> get_registered_sdl_enums()
{
	return
	{
		get_sdl_enum<EAccelerator>(),
		get_sdl_enum<ERayEnergyEstimator>(),
		get_sdl_enum<ESampleFilter>(),
		get_sdl_enum<math::EColorSpace>(),
		get_sdl_enum<math::EColorUsage>(),
	};
}

std::string_view get_config_directory(const EPhotonProject project)
{
	switch(project)
	{
	case EPhotonProject::ENGINE:        return PH_CONFIG_DIRECTORY "Engine/";
	case EPhotonProject::ENGINE_TEST:   return PH_CONFIG_DIRECTORY "EngineTest/";
	case EPhotonProject::SDL_GEN:       return PH_CONFIG_DIRECTORY "SDLGen/";
	case EPhotonProject::SDL_GEN_CLI:   return PH_CONFIG_DIRECTORY "SDLGenCLI/";
	case EPhotonProject::PHOTON_CLI:    return PH_CONFIG_DIRECTORY "PhotonCLI/";
	case EPhotonProject::PHOTON_EDITOR: return PH_CONFIG_DIRECTORY "PhotonEditor/";

	default: return PH_CONFIG_DIRECTORY;
	}

	PH_ASSERT_UNREACHABLE_SECTION();
	return "./";
}

std::string_view get_internal_resource_directory(const EPhotonProject project)
{
	switch(project)
	{
	case EPhotonProject::ENGINE:        return PH_INTERNAL_RESOURCE_DIRECTORY "Engine/";
	case EPhotonProject::ENGINE_TEST:   return PH_INTERNAL_RESOURCE_DIRECTORY "EngineTest/";
	case EPhotonProject::SDL_GEN:       return PH_INTERNAL_RESOURCE_DIRECTORY "SDLGen/";
	case EPhotonProject::SDL_GEN_CLI:   return PH_INTERNAL_RESOURCE_DIRECTORY "SDLGenCLI/";
	case EPhotonProject::PHOTON_CLI:    return PH_INTERNAL_RESOURCE_DIRECTORY "PhotonCLI/";
	case EPhotonProject::PHOTON_EDITOR: return PH_INTERNAL_RESOURCE_DIRECTORY "PhotonEditor/";

	default: return PH_INTERNAL_RESOURCE_DIRECTORY;
	}

	PH_ASSERT_UNREACHABLE_SECTION();
	return "./";
}

std::string_view get_core_resource_directory(const EPhotonProject project)
{
	switch(project)
	{
	case EPhotonProject::ENGINE:        return PH_RESOURCE_DIRECTORY "Engine/";
	case EPhotonProject::ENGINE_TEST:   return PH_RESOURCE_DIRECTORY "EngineTest/";
	case EPhotonProject::SDL_GEN:       return PH_RESOURCE_DIRECTORY "SDLGen/";
	case EPhotonProject::SDL_GEN_CLI:   return PH_RESOURCE_DIRECTORY "SDLGenCLI/";
	case EPhotonProject::PHOTON_CLI:    return PH_RESOURCE_DIRECTORY "PhotonCLI/";
	case EPhotonProject::PHOTON_EDITOR: return PH_RESOURCE_DIRECTORY "PhotonEditor/";

	default: return PH_RESOURCE_DIRECTORY;
	}

	PH_ASSERT_UNREACHABLE_SECTION();
	return "./";
}

}// end namespace ph
