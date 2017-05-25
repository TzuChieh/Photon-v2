#include "Api/init_and_exit.h"
#include "Api/ApiDatabase.h"

// geometries
#include "Actor/Geometry/Geometry.h"
#include "Actor/Geometry/GSphere.h"
#include "Actor/Geometry/GRectangle.h"
#include "Actor/Geometry/GTriangleMesh.h"

// materials
#include "Actor/Material/Material.h"
#include "Actor/Material/MatteOpaque.h"
#include "Actor/Material/AbradedOpaque.h"
#include "Actor/Material/AbradedTranslucent.h"

// light sources
#include "Actor/LightSource/LightSource.h"
#include "Actor/LightSource/AreaSource.h"

// actors
#include "Actor/Actor.h"
#include "Actor/PhysicalActor.h"
#include "Actor/AModel.h"
#include "Actor/ALight.h"

// cameras
#include "Core/Camera/Camera.h"
#include "Core/Camera/PerspectiveCamera.h"
#include "Core/Camera/PinholeCamera.h"
#include "Core/Camera/ThinLensCamera.h"

// films
#include "Core/Camera/Film.h"

// samplers
#include "Core/SampleGenerator/PixelJitterSampleGenerator.h"

// integrators
#include "Core/Integrator/Integrator.h"
#include "Core/Integrator/BackwardLightIntegrator.h"
#include "Core/Integrator/BackwardMisIntegrator.h"
#include "Core/Integrator/BackwardPathIntegrator.h"
#include "Core/Integrator/LightTracingIntegrator.h"
#include "Core/Integrator/NormalBufferIntegrator.h"

#include <iostream>

namespace ph
{

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

	// materials
	register_command_interface<Material>();
	register_command_interface<MatteOpaque>();
	register_command_interface<AbradedOpaque>();
	register_command_interface<AbradedTranslucent>();

	// light sources
	register_command_interface<LightSource>();
	register_command_interface<AreaSource>();

	// actors
	register_command_interface<Actor>();
	register_command_interface<PhysicalActor>();
	register_command_interface<AModel>();
	register_command_interface<ALight>();

	// cameras
	register_command_interface<Camera>();
	register_command_interface<PerspectiveCamera>();
	register_command_interface<PinholeCamera>();
	register_command_interface<ThinLensCamera>();

	// films
	register_command_interface<Film>();

	// samplers
	register_command_interface<PixelJitterSampleGenerator>();

	// integrators
	register_command_interface<Integrator>();
	register_command_interface<BackwardLightIntegrator>();
	register_command_interface<BackwardMisIntegrator>();
	register_command_interface<BackwardPathIntegrator>();
	register_command_interface<LightTracingIntegrator>();
	register_command_interface<NormalBufferIntegrator>();

	return true;
}

bool exit_api_database()
{
	ApiDatabase::clear();

	return true;
}

}// end namespace ph