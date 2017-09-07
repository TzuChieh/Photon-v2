#pragma once

#include "World/Scene.h"
#include "Core/Intersectable/Intersector.h"
#include "World/LightSampler/LightSampler.h"

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

bool Scene::isIntersecting(const Ray& ray, Intersection* out_intersection) const
{
	return m_intersector->isIntersecting(ray, out_intersection);
}

bool Scene::isIntersecting(const Ray& ray) const
{
	return m_intersector->isIntersecting(ray);
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