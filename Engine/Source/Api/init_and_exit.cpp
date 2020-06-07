#include "Api/init_and_exit.h"
#include "Api/ApiDatabase.h"
#include "Core/Quantity/ColorSpace.h"
#include "Common/Logger.h"
#include "DataIO/PictureSaver.h"

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

#include <iostream>

namespace ph
{

namespace
{
	Logger logger(LogSender("Init & Exit"));
}

bool init_core_infrastructure()
{
	logger.log("initializing color transform functions...");
	ColorSpace::init();

	if(!PictureSaver::init())
	{
		logger.log(ELogLevel::WARNING_MED,
			"picture saver init failed");
		return false;
	}

	return true;
}

template<typename T>
void register_command_interface()
{
	TCommandInterface<T>::registerInterface();
}

bool init_command_parser()
{
	// geometries
	register_command_interface<Geometry>();
	register_command_interface<GSphere>();
	register_command_interface<GRectangle>();
	register_command_interface<GTriangleMesh>();
	register_command_interface<GMengerSponge>();
	register_command_interface<GCuboid>();
	register_command_interface<GEmpty>();
	register_command_interface<GeometrySoup>();

	// materials
	register_command_interface<Material>();
	register_command_interface<MatteOpaque>();
	register_command_interface<AbradedOpaque>();
	register_command_interface<AbradedTranslucent>();
	register_command_interface<IdealSubstance>();
	register_command_interface<BinaryMixedSurfaceMaterial>();
	register_command_interface<FullMaterial>();
	register_command_interface<VAbsorptionOnly>();
	register_command_interface<LayeredSurface>();
	register_command_interface<ThinFilm>();

	// light sources
	register_command_interface<LightSource>();
	register_command_interface<ModelSource>();
	register_command_interface<AreaSource>();
	register_command_interface<PointSource>();
	register_command_interface<SphereSource>();
	register_command_interface<RectangleSource>();
	register_command_interface<IesAttenuatedSource>();

	// motions
	register_command_interface<MotionSource>();
	register_command_interface<ConstantVelocityMotion>();

	// actors
	register_command_interface<Actor>();
	register_command_interface<PhysicalActor>();
	register_command_interface<AModel>();
	register_command_interface<ALight>();
	register_command_interface<ADome>();
	register_command_interface<APhantomModel>();
	register_command_interface<ATransformedInstance>();

	// images
	register_command_interface<Image>();
	register_command_interface<ConstantImage>();
	register_command_interface<LdrPictureImage>();
	register_command_interface<HdrPictureImage>();
	register_command_interface<RealMathImage>();
	register_command_interface<CheckerboardImage>();
	register_command_interface<GradientImage>();

	// receivers
	register_command_interface<Receiver>();
	register_command_interface<PerspectiveReceiver>();
	register_command_interface<PinholeCamera>();
	register_command_interface<ThinLensCamera>();
	register_command_interface<EnvironmentCamera>();
	register_command_interface<RadiantFluxPanel>();
	register_command_interface<RadiantFluxPanelArray>();
	
	// sample generators
	register_command_interface<SampleGenerator>();
	register_command_interface<SGUniformRandom>();
	register_command_interface<SGStratified>();
	register_command_interface<SGHalton>();

	// renderers
	register_command_interface<Renderer>();
	register_command_interface<SamplingRenderer>();
	register_command_interface<EqualSamplingRenderer>();
	register_command_interface<AdaptiveSamplingRenderer>();
	register_command_interface<PMRenderer>();
	register_command_interface<AttributeRenderer>();

	// options
	register_command_interface<CookSettings>();
	register_command_interface<EngineOption>();

	return true;
}

bool exit_api_database()
{
	ApiDatabase::clear();

	return true;
}

}// end namespace ph