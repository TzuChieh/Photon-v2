#include "ph_cpp_core.h"
#include "Api/init_and_exit.h"

// geometries
#include "Actor/Geometry/Geometry.h"
#include "Actor/Geometry/GSphere.h"
#include "Actor/Geometry/GRectangle.h"
#include "Actor/Geometry/GTriangleMesh.h"
#include "Actor/Geometry/GMengerSponge.h"
#include "Actor/Geometry/GCuboid.h"
#include "Actor/Geometry/GEmpty.h"
#include "Actor/Geometry/GeometrySoup.h"

// materials
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

// light sources
#include "Actor/LightSource/LightSource.h"
#include "Actor/LightSource/ModelSource.h"
#include "Actor/LightSource/AreaSource.h"
#include "Actor/LightSource/PointSource.h"
#include "Actor/LightSource/SphereSource.h"
#include "Actor/LightSource/RectangleSource.h"
#include "Actor/LightSource/IesAttenuatedSource.h"

// motions
#include "Actor/MotionSource/MotionSource.h"
#include "Actor/MotionSource/ConstantVelocityMotion.h"

// actors
#include "Actor/Actor.h"
#include "Actor/PhysicalActor.h"
#include "Actor/AModel.h"
#include "Actor/ALight.h"
#include "Actor/ADome.h"
#include "Actor/APhantomModel.h"
#include "Actor/ATransformedInstance.h"
#include "Actor/Dome/AImageDome.h"
#include "Actor/Dome/APreethamDome.h"

// images
#include "Actor/Image/Image.h"
#include "Actor/Image/ConstantImage.h"
#include "Actor/Image/LdrPictureImage.h"
#include "Actor/Image/HdrPictureImage.h"
#include "Actor/Image/RealMathImage.h"
#include "Actor/Image/CheckerboardImage.h"
#include "Actor/Image/GradientImage.h"

// receivers
#include "Core/Receiver/Receiver.h"
#include "Core/Receiver/PerspectiveReceiver.h"
#include "Core/Receiver/PinholeCamera.h"
#include "Core/Receiver/ThinLensCamera.h"
#include "Core/Receiver/EnvironmentCamera.h"
#include "Core/Receiver/RadiantFluxPanel.h"
#include "Core/Receiver/RadiantFluxPanelArray.h"

// sample generators
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/SampleGenerator/SGUniformRandom.h"
#include "Core/SampleGenerator/SGStratified.h"
#include "Core/SampleGenerator/SGHalton.h"

// renderers
#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/Sampling/SamplingRenderer.h"
#include "Core/Renderer/Sampling/EqualSamplingRenderer.h"
#include "Core/Renderer/Sampling/AdaptiveSamplingRenderer.h"
#include "Core/Renderer/PM/PMRenderer.h"
#include "Core/Renderer/Attribute/AttributeRenderer.h"

// options
#include "World/CookSettings.h"
#include "Core/EngineOption.h"

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
	// be done in strange places later (which can cause problems).
	const std::vector<const SdlClass*> sdlClasses = get_registered_sdl_classes();
	logger.log(ELogLevel::NOTE_MED,
		"initialized " + std::to_string(sdlClasses.size()) + " SDL class instances");

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
	return &T::getSdlClass();
}

}

std::vector<const SdlClass*> get_registered_sdl_classes()
{
	return
	{
		// Materials
		get_sdl_class<Material>(),
		get_sdl_class<SurfaceMaterial>(),
		get_sdl_class<MatteOpaque>(),
	};
}

}// end namespace ph
