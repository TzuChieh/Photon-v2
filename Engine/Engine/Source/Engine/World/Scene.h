#pragma once

#include "Engine/Math/math_fwd.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Core/Quantity/TimeStep.h"

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
*/
class Scene final
{
public:
	Scene();
	Scene(const Intersector* intersector, const EmitterSampler* emitterSampler, TimeStep timeStep);

	bool isOccluding(const Ray& ray) const;
	bool isIntersecting(const Ray& ray, HitProbe* out_probe) const;

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

	/*! @brief Set the primitive to use when no other intersection is found.
	*/
	void setBackgroundPrimitive(const Primitive* const primitive);

	/*!
	Background primitive uses only metadata at slot 0.
	*/
	const Primitive* getBackgroundPrimitive() const;

	const VolumeBehavior* getBackgroundVolumeBehavior() const;

	/*! @brief Time interval this scene was cooked for.
	*/
	const TimeStep& getTimeStep() const;

private:
	const Intersector*    m_intersector;
	const EmitterSampler* m_emitterSampler;
	const Primitive*      m_backgroundPrimitive;
	TimeStep              m_timeStep;
};

// In-header Implementations:

inline void Scene::setBackgroundPrimitive(const Primitive* const primitive)
{
	m_backgroundPrimitive = primitive;
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
