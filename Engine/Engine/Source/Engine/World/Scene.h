#pragma once

#include "Engine/Math/math_fwd.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Core/Quantity/TimeStep.h"
#include "Engine/Core/Intersection/Intersector.h"
#include "Engine/Core/Emitter/SurfaceEmitter.h"
#include "Engine/Core/SurfaceHit.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

namespace ph
{

class Intersector;
class EmitterSampler;
class HitProbe;
class DirectEnergySampleQuery;
class DirectEnergyPdfQuery;
class EnergyEmissionSampleQuery;
class Ray;
class Emitter;
class Primitive;
class SampleFlow;
class VolumeBehavior;

/*! @brief A unified interface for accessing cooked content in a visual world.
Input data are fixed at construction and must outlive the scene.
*/
class Scene final
{
public:
	Scene(
		const Intersector* intersector,
		const EmitterSampler* emitterSampler,
		TimeStep timeStep,
		const Primitive* backgroundPrimitive = nullptr,
		const Intersector* nonBlockingLightIntersector = nullptr);

	bool isOccluding(const Ray& ray) const;
	bool isIntersecting(const Ray& ray, HitProbe* out_probe) const;

	/*! @brief Visit nonblocking emitter hits within a ray segment.
	@tparam FEATURE Allowed features. An emitter must enable at least one.
	@param visitor Called as `visitor(SurfaceHit& emitterHit)`; the hit is local to the callback.
	*/
	template<EEmitterFeatureSet FEATURE = EEmitterFeatureSet::Default, typename Visitor>
	void forEachNonBlockingEmitterHit(
		const Ray& ray,
		Visitor&& visitor) const;

	const Emitter* pickEmitter(SampleFlow& sampleFlow, real* out_PDF) const;

	/*! @brief Sample direct lighting for a target position.
	@note Generates hit event (with `DirectEnergySampleOutput::getObservationRay()` and `probe`).
	*/
	void genDirectSample(
		DirectEnergySampleQuery& query, 
		SampleFlow& sampleFlow,
		HitProbe& probe) const;

	/*! @brief Calculate the PDF of direct lighting for a target position.
	*/
	void calcDirectPdf(DirectEnergyPdfQuery& query) const;

	/*! @brief Emit a ray that carries some amount of energy from an emitter.
	@note Generates hit event (with `EnergyEmissionSampleOutput::getEmittedRay()` and `probe`).
	*/
	void emitRay(
		EnergyEmissionSampleQuery& query,
		SampleFlow& sampleFlow,
		HitProbe& probe) const;

	/*! @brief The primitive to use when no other intersection is found.
	Background primitive uses only metadata at slot 0.
	*/
	const Primitive* getBackgroundPrimitive() const;

	const VolumeBehavior* getBackgroundVolumeBehavior() const;

	/*! @brief Time interval this scene was cooked for.
	*/
	const TimeStep& getTimeStep() const;

private:
	const Intersector* const m_intersector;
	const Intersector* const m_nonBlockingLightIntersector;
	const EmitterSampler* const m_emitterSampler;
	const Primitive* const m_backgroundPrimitive;
	const TimeStep m_timeStep;
};

// In-header Implementations:

template<EEmitterFeatureSet FEATURE, typename Visitor>
inline void Scene::forEachNonBlockingEmitterHit(
	const Ray& ray,
	Visitor&& visitor) const
{
	if(m_nonBlockingLightIntersector)
	{
		m_nonBlockingLightIntersector->forEachIntersection(ray,
			[&visitor](const Ray& hitRay, const HitProbe& probe)
			{
				SurfaceHit Xe(hitRay, probe, ESurfaceHitReason::IncidentRay);
				if(Xe.getSurfaceEmitter().getFeatureSet().hasAny(FEATURE))
				{
					visitor(Xe);
				}
			});
	}
}

inline const Primitive* Scene::getBackgroundPrimitive() const
{
	return m_backgroundPrimitive;
}

inline const TimeStep& Scene::getTimeStep() const
{
	return m_timeStep;
}

}// end namespace ph
