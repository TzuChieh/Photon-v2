#pragma once

#include "Math/TVector3.h"
#include "Core/Ray.h"
#include "Core/Quantity/Time.h"
#include "Core/SurfaceHit.h"
#include "Core/LTA/PDF.h"
#include "Math/Color/Spectrum.h"

#include <Common/assertion.h>

namespace ph
{

class Primitive;

/*! @brief Input for `DirectEnergySampleQuery`.
*/
class DirectEnergySampleInput final
{
public:
	void set(const SurfaceHit& X);

	/*!
	@return Information about the energy receiving position.
	*/
	const SurfaceHit& getX() const;

	/*!
	@return The position that receives energy.
	*/
	math::Vector3R getTargetPos() const;

private:
	SurfaceHit m_X;
#if PH_DEBUG
	bool       m_hasSet{false};
#endif
};

/*! @brief Output for `DirectEnergySampleQuery`.
*/
class DirectEnergySampleOutput final
{
public:
	void setEmitPos(const math::Vector3R& emitPos);
	void setEmittedEnergy(const math::Spectrum& emittedEnergy);
	void setPdf(const lta::PDF& pdf);
	void setSrcPrimitive(const Primitive* srcPrimitive);
	void setObservationRay(const Ray& observationRay);

	/*!
	@return The position that emits energy.
	*/
	const math::Vector3R& getEmitPos() const;

	/*! @brief The sampled emitted energy of. Does not contain any path weighting.
	*/
	const math::Spectrum& getEmittedEnergy() const;

	real getPdfW() const;
	const lta::PDF& getPdf() const;
	const Primitive* getSrcPrimitive() const;

	/*! @brief Get the ray from target position to sampled emitting position.
	If target position or emitting position is required, it is recommended to use
	`DirectEnergySampleInput::getTargetPos()` and `getEmitPos()`, respectively. Using the head and
	tail of the observation ray tends to have larger numerical error.
	*/
	const Ray& getObservationRay() const;

	/*! @brief Checks the validity of the sample.
	@return `true` if the sample is valid. `false` if the sample failed. A failed sample cannot be
	used. A typical way to fail is that sampling is not even supported/possible.
	*/
	operator bool () const;

private:
	math::Vector3R   m_emitPos{0};
	math::Spectrum   m_emittedEnergy{0};
	lta::PDF         m_pdf{};
	const Primitive* m_srcPrimitive{nullptr};
	Ray              m_observationRay{};
};

/*! @brief Information for generating a sample for direct energy estimation.
*/
class DirectEnergySampleQuery final
{
public:
	using Input  = DirectEnergySampleInput;
	using Output = DirectEnergySampleOutput;

	Input  inputs;
	Output outputs;

	DirectEnergySampleQuery() = default;

	math::Vector3R getTargetToEmit() const;
	math::Vector3R getEmitToTarget() const;
};

inline void DirectEnergySampleInput::set(const SurfaceHit& X)
{
	m_X = X;

#if PH_DEBUG
	m_hasSet = true;
#endif
}

inline const SurfaceHit& DirectEnergySampleInput::getX() const
{
	PH_ASSERT(m_hasSet);
	return m_X;
}

inline math::Vector3R DirectEnergySampleInput::getTargetPos() const
{
	return getX().getPos();
}

inline void DirectEnergySampleOutput::setEmitPos(const math::Vector3R& emitPos)
{
	m_emitPos = emitPos;
}

inline void DirectEnergySampleOutput::setEmittedEnergy(const math::Spectrum& emittedEnergy)
{
	m_emittedEnergy = emittedEnergy;
}

inline void DirectEnergySampleOutput::setPdf(const lta::PDF& pdf)
{
	m_pdf = pdf;
}

inline void DirectEnergySampleOutput::setSrcPrimitive(const Primitive* srcPrimitive)
{
	m_srcPrimitive = srcPrimitive;
}

inline void DirectEnergySampleOutput::setObservationRay(const Ray& observationRay)
{
	m_observationRay = observationRay;
}

inline const math::Vector3R& DirectEnergySampleOutput::getEmitPos() const
{
	PH_ASSERT(*this);
	return m_emitPos;
}

inline const math::Spectrum& DirectEnergySampleOutput::getEmittedEnergy() const
{
	PH_ASSERT(*this);
	return m_emittedEnergy;
}

inline real DirectEnergySampleOutput::getPdfW() const
{
	PH_ASSERT(*this);
	return m_pdf.getPdfW();
}

inline const lta::PDF& DirectEnergySampleOutput::getPdf() const
{
	PH_ASSERT(*this);
	return m_pdf;
}

inline const Primitive* DirectEnergySampleOutput::getSrcPrimitive() const
{
	PH_ASSERT(*this);
	PH_ASSERT(m_srcPrimitive);
	return m_srcPrimitive;
}

inline const Ray& DirectEnergySampleOutput::getObservationRay() const
{
	PH_ASSERT(*this);
	return m_observationRay;
}

inline DirectEnergySampleOutput::operator bool () const
{
	return !m_pdf.isEmpty();
}

inline math::Vector3R DirectEnergySampleQuery::getTargetToEmit() const
{
	return outputs.getEmitPos() - inputs.getTargetPos();
}

inline math::Vector3R DirectEnergySampleQuery::getEmitToTarget() const
{
	return inputs.getTargetPos() - outputs.getEmitPos();
}

}// end namespace ph
