#include "Core/Integrator/LightTracingIntegrator.h"
#include "Core/Ray.h"
#include "World/World.h"
#include "World/Intersector.h"
#include "World/LightSampler/LightSampler.h"
#include "Math/Vector3f.h"
#include "Core/Intersection.h"
#include "Core/Sample/SurfaceSample.h"
#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/BSDFcos.h"
#include "Core/Primitive/Primitive.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Math/Math.h"
#include "Math/random_number.h"
#include "Core/Sample/DirectLightSample.h"
#include "Camera/Camera.h"

#include <iostream>

#define RAY_DELTA_DIST 0.0001f
#define MAX_RAY_BOUNCES 10000
//#define MAX_RAY_BOUNCES 2

namespace ph
{

LightTracingIntegrator::~LightTracingIntegrator() = default;

void LightTracingIntegrator::update(const World& world)
{
	// update nothing
}

void LightTracingIntegrator::radianceAlongRay(const Sample& sample, const World& world, const Camera& camera, std::vector<SenseEvent>& out_senseEvents) const
{
	Ray ray;
	camera.genSensingRay(sample, &ray);

	// convenient variables
	const Intersector&       intersector = world.getIntersector();
	const LightSampler&      lightSampler = world.getLightSampler();
	const PrimitiveMetadata* metadata = nullptr;
	const BSDFcos*           bsdfCos = nullptr;

	/*Intersection cameraIntersection;
	Ray cameraRay(ray.getOrigin(), ray.getDirection(), RAY_T_EPSILON, RAY_T_MAX);
	if(!intersector.isIntersecting(cameraRay, &cameraIntersection))
	{
		return;
	}

	Vector3f cameraV = cameraRay.getDirection().mul(-1.0f);
	if(cameraIntersection.getHitGeoNormal().dot(cameraV) * cameraIntersection.getHitSmoothNormal().dot(cameraV) <= 0.0f)
	{
		return;
	}

	Vector3f cameraImportanceWe;
	Vector2f filmCoord;
	float32 cameraRayPdfW;
	camera.evalEmittedImportanceAndPdfW(cameraIntersection.getHitPosition(), &filmCoord, &cameraImportanceWe, &cameraRayPdfW);
	if(cameraImportanceWe.allZero() || cameraRayPdfW <= 0.0f)
	{
		return;
	}*/

	
	float32 emitterPickPdf;
	const Emitter* emitter = lightSampler.pickEmitter(&emitterPickPdf);
	if(!emitter || emitterPickPdf <= 0.0f)
	{
		return;
	}

	Ray emitterRay;
	Vector3f emitterRadianceLe;
	float32 emitterPdfA;
	float32 emitterPdfW;
	emitter->genSensingRay(&emitterRay, &emitterRadianceLe, &emitterPdfA, &emitterPdfW);
	if(emitterRadianceLe.allZero() || emitterPdfA <= 0.0f || emitterPdfW <= 0.0f)
	{
		return;
	}

	Intersection lightIntersection;
}

// NaNs will be clamped to 0
void LightTracingIntegrator::rationalClamp(Vector3f& value)
{
	value.x = value.x > 0.0f && value.x < 10000.0f ? value.x : 0.0f;
	value.y = value.y > 0.0f && value.y < 10000.0f ? value.y : 0.0f;
	value.z = value.z > 0.0f && value.z < 10000.0f ? value.z : 0.0f;
}

}// end namespace ph