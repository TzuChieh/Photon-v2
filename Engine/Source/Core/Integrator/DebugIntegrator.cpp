#include "Core/Integrator/DebugIntegrator.h"
#include "Core/Ray.h"
#include "Core/Intersection.h"
#include "World/Scene.h"
#include "Math/TVector3.h"

namespace ph
{

DebugIntegrator::~DebugIntegrator() = default;

void DebugIntegrator::update(const Scene& scene)
{
	// update nothing
}

void DebugIntegrator::radianceAlongRay(const Ray& ray, const RenderWork& data, std::vector<SenseEvent>& out_senseEvents) const
{
	// just output camera sample location as green dots
	SpectralStrength radiance;
	radiance.setRgb(Vector3R(0, 1, 0));
	out_senseEvents.push_back(SenseEvent(radiance));
}

// command interface

DebugIntegrator::DebugIntegrator(const InputPacket& packet) :
	Integrator(packet)
{

}

SdlTypeInfo DebugIntegrator::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_INTEGRATOR, "debug");
}

std::unique_ptr<DebugIntegrator> DebugIntegrator::ciLoad(const InputPacket& packet)
{
	return std::make_unique<DebugIntegrator>(packet);
}

ExitStatus DebugIntegrator::ciExecute(const std::shared_ptr<DebugIntegrator>& targetResource, 
                                      const std::string& functionName, 
                                      const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph