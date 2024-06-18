#include "World/Scene.h"
#include "Core/Emitter/Sampler/EmitterSampler.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Core/Ray.h"
#include "Core/Intersection/Intersector.h"
#include "Core/Intersection/Primitive.h"
#include "Core/Emitter/Emitter.h"
#include "Core/Emitter/Query/EnergyEmissionSampleQuery.h"

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

	*out_probe = HitProbe{};
	if(m_intersector->isIntersecting(ray, *out_probe))
	{
		return true;
	}
	else if(m_backgroundPrimitive)
	{
		*out_probe = HitProbe{};
		return m_backgroundPrimitive->isIntersecting(ray, *out_probe);
	}

	return false;
}

bool Scene::isOccluding(const Ray& ray) const
{
	PH_ASSERT(ray.getOrigin().isFinite() && ray.getDir().isFinite());

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

void Scene::genDirectSample(
	DirectEnergySampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	m_emitterSampler->genDirectSample(query, sampleFlow, probe);
}

void Scene::calcDirectPdf(DirectEnergyPdfQuery& query) const
{
	m_emitterSampler->calcDirectPdf(query);
}

void Scene::emitRay(
	EnergyEmissionSampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	real pickPdf;
	const Emitter* emitter = m_emitterSampler->pickEmitter(sampleFlow, &pickPdf);

	PH_ASSERT(emitter);
	emitter->emitRay(query, sampleFlow, probe);
	if(!query.outputs)
	{
		return;
	}

	query.outputs.setPdf(query.outputs.getPdfPos() * pickPdf, query.outputs.getPdfDir());
}

}// end namespace ph
