#include "World/Scene.h"
#include "Core/Intersectable/Intersector.h"
#include "Core/Emitter/Sampler/EmitterSampler.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Core/Ray.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Emitter/Emitter.h"

#include <Common/assertion.h>

#include <limits>

namespace ph
{

Scene::Scene() : 
	m_intersector        (nullptr), 
	m_emitterSampler     (nullptr),
	m_backgroundPrimitive(nullptr)
{}

Scene::Scene(
	const Intersector* const    intersector,
	const EmitterSampler* const emitterSampler) :

	m_intersector        (intersector), 
	m_emitterSampler     (emitterSampler),
	m_backgroundPrimitive(nullptr)
{
	PH_ASSERT(intersector);
	PH_ASSERT(emitterSampler);
}

bool Scene::isIntersecting(const Ray& ray, HitProbe* const out_probe) const
{
	PH_ASSERT(out_probe);

	out_probe->clear();
	if(m_intersector->isIntersecting(ray, *out_probe))
	{
		return true;
	}
	else if(m_backgroundPrimitive)
	{
		out_probe->clear();
		return m_backgroundPrimitive->isIntersecting(ray, *out_probe);
	}

	return false;
}

bool Scene::isOccluding(const Ray& ray) const
{
	PH_ASSERT(ray.getOrigin().isFinite() && ray.getDirection().isFinite());

	if(m_intersector->isOccluding(ray))
	{
		return true;
	}
	else if(m_backgroundPrimitive)
	{
		return m_backgroundPrimitive->isOccluding(ray);
	}

	return false;
}

const Emitter* Scene::pickEmitter(SampleFlow& sampleFlow, real* const out_PDF) const
{
	PH_ASSERT(out_PDF);

	return m_emitterSampler->pickEmitter(sampleFlow, out_PDF);
}

void Scene::genDirectSample(DirectEnergySampleQuery& query, SampleFlow& sampleFlow) const
{
	m_emitterSampler->genDirectSample(query, sampleFlow);
}

real Scene::calcDirectPdfW(const SurfaceHit& emitPos, const math::Vector3R& targetPos) const
{
	return m_emitterSampler->calcDirectPdfW(emitPos, targetPos);
}

void Scene::emitRay(SampleFlow& sampleFlow, Ray* out_ray, math::Spectrum* out_Le, math::Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const
{
	real pickPdf;
	const Emitter* emitter = m_emitterSampler->pickEmitter(sampleFlow, &pickPdf);

	emitter->emitRay(sampleFlow, out_ray, out_Le, out_eN, out_pdfA, out_pdfW);
	*out_pdfA *= pickPdf;
}

}// end namespace ph
