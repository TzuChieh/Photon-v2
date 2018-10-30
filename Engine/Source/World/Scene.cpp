#include "World/Scene.h"
#include "Core/Intersectable/Intersector.h"
#include "Core/Emitter/Sampler/EmitterSampler.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Common/assertion.h"
#include "Core/Ray.h"
#include "Core/Intersectable/Primitive.h"

#include <limits>

namespace ph
{

Scene::Scene() : 
	m_intersector(nullptr), m_emitterSampler(nullptr),

	m_backgroundEmitterPrimitive(nullptr)
{}

Scene::Scene(const Intersector* intersector, const EmitterSampler* emitterSampler) :
	m_intersector(intersector), m_emitterSampler(emitterSampler),

	m_backgroundEmitterPrimitive(nullptr)
{}

bool Scene::isIntersecting(const Ray& ray, HitProbe* const out_probe) const
{
	PH_ASSERT(out_probe);

	out_probe->clear();
	if(m_intersector->isIntersecting(ray, *out_probe))
	{
		return true;
	}
	else if(m_backgroundEmitterPrimitive)
	{
		return m_backgroundEmitterPrimitive->isIntersecting(ray, *out_probe);
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
	else if(m_backgroundEmitterPrimitive)
	{
		return m_backgroundEmitterPrimitive->isIntersecting(ray);
	}

	return false;
}

const Emitter* Scene::pickEmitter(real* const out_PDF) const
{
	PH_ASSERT(out_PDF);

	return m_emitterSampler->pickEmitter(out_PDF);
}

void Scene::genDirectSample(DirectLightSample& sample) const
{
	m_emitterSampler->genDirectSample(sample);
}

real Scene::calcDirectPdfW(const SurfaceHit& emitPos, const Vector3R& targetPos) const
{
	return m_emitterSampler->calcDirectPdfW(emitPos, targetPos);
}

}// end namespace ph