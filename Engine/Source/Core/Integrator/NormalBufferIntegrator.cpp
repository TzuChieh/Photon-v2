#include "Core/Integrator/NormalBufferIntegrator.h"
#include "Core/Ray.h"
#include "Core/IntersectionProbe.h"
#include "Core/IntersectionDetail.h"
#include "World/Scene.h"
#include "Math/TVector3.h"
#include "Core/Quantity/SpectralStrength.h"

namespace ph
{

NormalBufferIntegrator::~NormalBufferIntegrator() = default;

void NormalBufferIntegrator::update(const Scene& scene)
{
	// update nothing
}

void NormalBufferIntegrator::radianceAlongRay(const Ray& ray, const RenderWork& data, std::vector<SenseEvent>& out_senseEvents) const
{
	/*Ray ray;
	data.camera->genSensingRay(sample, &ray);*/
	
	// reverse tracing
	const Ray tracingRay(ray.getOrigin(), ray.getDirection().mul(-1.0f), 0.0001_r, Ray::MAX_T);// HACK: hard-coded number
	
	SpectralStrength radiance;
	IntersectionProbe probe;
	if(data.scene->isIntersecting(tracingRay, &probe))
	{
		IntersectionDetail detail;
		probe.calcIntersectionDetail(tracingRay, &detail);
		radiance.setRgb(detail.getHitSmoothNormal());
	}
	else
	{
		radiance.setRgb(Vector3R(0));
	}

	out_senseEvents.push_back(SenseEvent(/*sample.m_cameraX, sample.m_cameraY, */radiance));
}

// command interface

NormalBufferIntegrator::NormalBufferIntegrator(const InputPacket& packet) :
	Integrator(packet)
{

}

SdlTypeInfo NormalBufferIntegrator::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_INTEGRATOR, "surface-normal");
}

void NormalBufferIntegrator::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<NormalBufferIntegrator>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<NormalBufferIntegrator> NormalBufferIntegrator::ciLoad(const InputPacket& packet)
{
	return std::make_unique<NormalBufferIntegrator>(packet);
}

}// end namespace ph