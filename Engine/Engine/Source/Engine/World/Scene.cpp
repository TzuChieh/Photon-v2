#include "Engine/World/Scene.h"
#include "Engine/Core/Emitter/Sampler/EmitterSampler.h"
#include "Engine/Core/HitProbe.h"
#include "Engine/Core/HitDetail.h"
#include "Engine/Core/Ray.h"
#include "Engine/Core/Intersection/Intersector.h"
#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Core/Emitter/Emitter.h"
#include "Engine/Core/Emitter/Query/EnergyEmissionSampleQuery.h"

#include <Common/assertion.h>

#include <limits>

namespace ph
{

Scene::Scene()
	: m_intersector        (nullptr)
	, m_emitterSampler     (nullptr)
	, m_backgroundPrimitive(nullptr)
{}

Scene::Scene(
	const Intersector* const    intersector,
	const EmitterSampler* const emitterSampler)

	: m_intersector        (intersector)
	, m_emitterSampler     (emitterSampler)
	, m_backgroundPrimitive(nullptr)
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
	else if(getBackgroundPrimitive())
	{
		*out_probe = HitProbe{};
		return getBackgroundPrimitive()->isIntersecting(ray, *out_probe);
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
	else if(getBackgroundPrimitive())
	{
		return getBackgroundPrimitive()->isOccluding(ray);
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

const VolumeBehavior* Scene::getBackgroundVolumeBehavior() const
{
	if(!getBackgroundPrimitive())
	{
		return nullptr;
	}

	const PrimitiveMetadata& metadata = getBackgroundPrimitive()->getMetadata();
	return &metadata.getInterior();
}

}// end namespace ph
