#include "ph_cpp_core.h"
#include "Api/init_and_exit.h"

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
#include "Actor/Image/ConstantImage.h"
#include "Actor/Image/LdrPictureImage.h"
#include "Actor/Image/HdrPictureImage.h"
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

namespace ph
{

namespace
{
	
const Logger logger(LogSender("C++ API"));

}

bool init_render_engine()
{
	if(!init_core_infrastructure())
	{
		logger.log(ELogLevel::FATAL_ERROR,
			"core infrastructure initialization failed");
		return false;
	}

	// Get SDL classes once here to initialize them--this is not required,
	// just to be safe as SDL class instances are lazy-constructed and may
	// be done in strange places/order later (which may cause problems).
	//
	const std::vector<const SdlClass*> sdlClasses = get_registered_sdl_classes();
	logger.log(ELogLevel::NOTE_MED,
		"initialized " + std::to_string(sdlClasses.size()) + " SDL class definitions");

	if(!init_command_parser())
	{
		logger.log(ELogLevel::FATAL_ERROR,
			"command parser initialization failed");
		return false;
	}

	return true;
}

bool exit_render_engine()
{
	if(!exit_api_database())
	{
		logger.log(ELogLevel::FATAL_ERROR,
			"C API database exiting failed");
		return false;
	}

	return true;
}

namespace
{

template<typename T>
const SdlClass* get_sdl_class()
{
	return T::getSdlClass();
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

		// Materials
		get_sdl_class<Material>(),
		get_sdl_class<SurfaceMaterial>(),
		get_sdl_class<MatteOpaque>(),

		// Light Sources
		get_sdl_class<LightSource>(),
		get_sdl_class<AreaSource>(),
		get_sdl_class<SphereSource>(),

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
	};
}

}// end namespace ph
