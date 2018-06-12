#include "Core/Integrator/NormalBufferIntegrator.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "World/Scene.h"
#include "Math/TVector3.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/SurfaceHit.h"

namespace ph
{

NormalBufferIntegrator::~NormalBufferIntegrator() = default;

//void NormalBufferIntegrator::update(const Scene& scene)
//{
//	// update nothing
//}
//
//void NormalBufferIntegrator::radianceAlongRay(const Ray& ray, const RenderWork& data, std::vector<SenseEvent>& out_senseEvents) const
//{
//	// reverse tracing
//	Ray tracingRay = Ray(ray).reverse();
//	tracingRay.setMinT(0.0002_r);// HACK: hard-coded number
//	tracingRay.setMaxT(std::numeric_limits<real>::max());
//
//	SpectralStrength radiance;
//	HitProbe probe;
//	if(data.scene->isIntersecting(tracingRay, &probe))
//	{
//		const SurfaceHit hit(tracingRay, probe);
//		radiance.setLinearSrgb(hit.getDetail().getShadingNormal());// FIXME: check color space
//	}
//	else
//	{
//		radiance.setLinearSrgb(Vector3R(0));// FIXME: check color space
//	}
//
//	out_senseEvents.push_back(SenseEvent(/*sample.m_cameraX, sample.m_cameraY, */radiance));
//}

// command interface

NormalBufferIntegrator::NormalBufferIntegrator(const InputPacket& packet) :
	Integrator(packet)
{}

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
	//return std::make_unique<NormalBufferIntegrator>(packet);
	return nullptr;
}

}// end namespace ph