#include "World/Scene.h"
#include "Core/Intersectable/Intersector.h"
#include "World/LightSampler/LightSampler.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Common/assertion.h"
#include "Core/Ray.h"
#include "Core/Intersectable/Primitive.h"

#include <limits>

namespace ph
{

Scene::Scene() : 
	m_intersector(nullptr), m_lightSampler(nullptr),

	m_backgroundEmitterPrimitive(nullptr)
{}

Scene::Scene(const Intersector* intersector, const LightSampler* lightSampler) : 
	m_intersector(intersector), m_lightSampler(lightSampler),

	m_backgroundEmitterPrimitive(nullptr)
{}

bool Scene::isIntersecting(const Ray& ray, HitProbe* const out_probe) const
{
	PH_ASSERT(out_probe != nullptr);

	out_probe->clear();
	if(m_intersector->isIntersecting(ray, *out_probe))
	{
		return true;
	}
	else if(m_backgroundEmitterPrimitive && 
	        ray.getMaxT() >= std::numeric_limits<real>::max())
	{
		out_probe->pushBaseHit(m_backgroundEmitterPrimitive, ray.getMaxT());
		return true;
	}

	return false;
}

bool Scene::isIntersecting(const Ray& ray) const
{
	PH_ASSERT(ray.getOrigin().isFinite() && ray.getDirection().isFinite());

	if(m_intersector->isIntersecting(ray))
	{
		return true;
	}
	else if(m_backgroundEmitterPrimitive &&
	        ray.getMaxT() >= std::numeric_limits<real>::max())
	{
		return true;
	}

	return false;
}

const Emitter* Scene::pickEmitter(real* const out_PDF) const
{
	PH_ASSERT(out_PDF != nullptr);

	return m_lightSampler->pickEmitter(out_PDF);
}

void Scene::genDirectSample(DirectLightSample& sample) const
{
	m_lightSampler->genDirectSample(sample);
}

real Scene::calcDirectPdfW(const SurfaceHit& emitPos, const Vector3R& targetPos) const
{
	return m_lightSampler->calcDirectPdfW(emitPos, targetPos);
}

}// end namespace ph