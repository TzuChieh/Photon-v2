#include "Api/init_and_exit.h"
#include "Api/ApiDatabase.h"
#include "Core/Quantity/ColorSpace.h"

// geometries
#include "Actor/Geometry/Geometry.h"
#include "Actor/Geometry/GSphere.h"
#include "Actor/Geometry/GRectangle.h"
#include "Actor/Geometry/GTriangleMesh.h"
#include "Actor/Geometry/GMengerSponge.h"
#include "Actor/Geometry/GInfiniteSphere.h"
#include "Actor/Geometry/GCuboid.h"
#include "Actor/Geometry/GEmpty.h"

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

// images
#include "Actor/Image/Image.h"
#include "Actor/Image/ConstantImage.h"
#include "Actor/Image/LdrPictureImage.h"
#include "Actor/Image/HdrPictureImage.h"
#include "Actor/Image/RealMathImage.h"
#include "Actor/Image/CheckerboardImage.h"

// cameras
#include "Core/Camera/Camera.h"
#include "Core/Camera/PerspectiveCamera.h"
#include "Core/Camera/PinholeCamera.h"
#include "Core/Camera/ThinLensCamera.h"

// films
#include "Core/Filmic/HdrRgbFilm.h"

// sample generators
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/SampleGenerator/SGUniformRandom.h"
#include "Core/SampleGenerator/SGStratified.h"

// estimators
#include "Core/Estimator/Estimator.h"
//#include "Core/Integrator/BackwardLightIntegrator.h"
#include "Core/Estimator/BNEEPTEstimator.h"
#include "Core/Estimator/BVPTEstimator.h"
//#include "Core/Integrator/LightTracingIntegrator.h"
//#include "Core/Integrator/NormalBufferIntegrator.h"
//#include "Core/Integrator/DebugIntegrator.h"

// renderers
#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/Sampling/SamplingRenderer.h"
#include "Core/Renderer/PM/PMRenderer.h"

// options
#include "World/CookSettings.h"

#include <iostream>

namespace ph
{

void init_core_infrastructure()
{
	std::cout << "initializing color transform functions" << std::endl;
	ColorSpace::init();
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
	register_command_interface<GInfiniteSphere>();
	register_command_interface<GCuboid>();
	register_command_interface<GEmpty>();

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

	// images
	register_command_interface<Image>();
	register_command_interface<ConstantImage>();
	register_command_interface<LdrPictureImage>();
	register_command_interface<HdrPictureImage>();
	register_command_interface<RealMathImage>();
	register_command_interface<CheckerboardImage>();

	// cameras
	register_command_interface<Camera>();
	register_command_interface<PerspectiveCamera>();
	register_command_interface<PinholeCamera>();
	register_command_interface<ThinLensCamera>();

	// films
	register_command_interface<HdrRgbFilm>();
	
	// sample generators
	register_command_interface<SampleGenerator>();
	register_command_interface<SGUniformRandom>();
	register_command_interface<SGStratified>();

	// estimators
	register_command_interface<Estimator>();
	//register_command_interface<BackwardLightIntegrator>();
	register_command_interface<BNEEPTEstimator>();
	register_command_interface<BVPTEstimator>();
	/*register_command_interface<LightTracingIntegrator>();
	register_command_interface<NormalBufferIntegrator>();
	register_command_interface<DebugIntegrator>();*/

	// renderers
	register_command_interface<Renderer>();
	register_command_interface<SamplingRenderer>();
	register_command_interface<PMRenderer>();

	// options
	register_command_interface<CookSettings>();

	return true;
}

bool exit_api_database()
{
	ApiDatabase::clear();

	return true;
}

}// end namespace ph