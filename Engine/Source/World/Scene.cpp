#pragma once

#include "World/Scene.h"
#include "Core/Intersectable/Intersector.h"
#include "World/LightSampler/LightSampler.h"
#include "Core/IntersectionProbe.h"

namespace ph
{

Scene::Scene() : 
	m_intersector(nullptr), m_lightSampler(nullptr)
{

}

Scene::Scene(const Intersector* intersector, const LightSampler* lightSampler) : 
	m_intersector(intersector), m_lightSampler(lightSampler)
{

}

bool Scene::isIntersecting(const Ray& ray, IntersectionProbe* const out_probe) const
{
	out_probe->clear();
	return m_intersector->isIntersecting(ray, *out_probe);
}

bool Scene::isIntersecting(const Ray& ray) const
{
	return m_intersector->isIntersecting(ray);
}

void Scene::calcIntersectionDetail(const Ray& ray, IntersectionProbe& probe,
                                   IntersectionDetail* const out_detail) const
{
	m_intersector->calcIntersectionDetail(ray, probe, out_detail);
}

const Emitter* Scene::pickEmitter(real* const out_PDF) const
{
	return m_lightSampler->pickEmitter(out_PDF);
}

void Scene::genDirectSample(DirectLightSample& sample) const
{
	m_lightSampler->genDirectSample(sample);
}

real Scene::calcDirectPdfW(const Vector3R& targetPos, const Vector3R& emitPos, const Vector3R& emitN, const Emitter* hitEmitter, const Primitive* hitPrim) const
{
	return m_lightSampler->calcDirectPdfW(targetPos, emitPos, emitN, hitEmitter, hitPrim);
}

}// end namespace ph