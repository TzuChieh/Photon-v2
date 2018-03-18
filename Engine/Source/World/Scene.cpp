#include "World/Scene.h"
#include "Core/Intersectable/Intersector.h"
#include "World/LightSampler/LightSampler.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Common/assertion.h"

namespace ph
{

Scene::Scene() : 
	m_intersector(nullptr), m_lightSampler(nullptr)
{}

Scene::Scene(const Intersector* intersector, const LightSampler* lightSampler) : 
	m_intersector(intersector), m_lightSampler(lightSampler)
{}

bool Scene::isIntersecting(const Ray& ray, HitProbe* const out_probe) const
{
	PH_ASSERT(out_probe != nullptr);

	out_probe->clear();
	return m_intersector->isIntersecting(ray, *out_probe);
}

bool Scene::isIntersecting(const Ray& ray) const
{
	return m_intersector->isIntersecting(ray);
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

real Scene::calcDirectPdfW(const Vector3R& targetPos, const Vector3R& emitPos, const Vector3R& emitN, const Emitter* hitEmitter, const Primitive* hitPrim) const
{
	PH_ASSERT(hitEmitter != nullptr && hitPrim != nullptr);

	return m_lightSampler->calcDirectPdfW(targetPos, emitPos, emitN, hitEmitter, hitPrim);
}

}// end namespace ph