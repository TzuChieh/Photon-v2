#pragma once

#include "Math/TVector3.h"
#include "Core/Ray.h"
#include "Core/Quantity/Time.h"
#include "Core/SurfaceHit.h"
#include "Math/Color/Spectrum.h"

#include <Common/assertion.h>

namespace ph
{

class Primitive;

class DirectEnergySampleInput final
{
public:
	void set(const SurfaceHit& X);
	void set(const math::Vector3R& targetPos, const Time& time);

	const math::Vector3R& getTargetPos() const;
	const Time& getTime() const;

private:
	math::Vector3R m_targetPos;
	Time m_time;
#if PH_DEBUG
	bool m_hasSet{false};
#endif
};

class DirectEnergySampleOutput final
{
public:
	void setEmitPos(const math::Vector3R& emitPos);
	void setEmittedEnergy(const math::Spectrum& emittedEnergy);
	void setPdfW(real pdfW);
	void setSrcPrimitive(const Primitive* srcPrimitive);
	void setObservationRay(const Ray& observationRay);
	void invalidate();

	const math::Vector3R& getEmitPos() const;
	const math::Spectrum& getEmittedEnergy() const;
	real getPdfW() const;
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
	real             m_pdfW{0};
	const Primitive* m_srcPrimitive{nullptr};
	Ray              m_observationRay{};
};

class DirectEnergySampleQuery final
{
public:
	using Input  = DirectEnergySampleInput;
	using Output = DirectEnergySampleOutput;

	Input  inputs;
	Output outputs;

	DirectEnergySampleQuery() = default;
};

inline void DirectEnergySampleInput::set(const SurfaceHit& X)
{
	set(X.getPosition(), X.getTime());
}

inline void DirectEnergySampleInput::set(const math::Vector3R& targetPos, const Time& time)
{
	m_targetPos = targetPos;
	m_time = time;

#if PH_DEBUG
	m_hasSet = true;
#endif
}

inline const math::Vector3R& DirectEnergySampleInput::getTargetPos() const
{
	PH_ASSERT(m_hasSet);
	return m_targetPos;
}

inline const Time& DirectEnergySampleInput::getTime() const
{
	PH_ASSERT(m_hasSet);
	return m_time;
}

inline void DirectEnergySampleOutput::setEmitPos(const math::Vector3R& emitPos)
{
	m_emitPos = emitPos;
}

inline void DirectEnergySampleOutput::setEmittedEnergy(const math::Spectrum& emittedEnergy)
{
	m_emittedEnergy = emittedEnergy;
}

inline void DirectEnergySampleOutput::setPdfW(const real pdfW)
{
	m_pdfW = pdfW;
}

inline void DirectEnergySampleOutput::setSrcPrimitive(const Primitive* srcPrimitive)
{
	m_srcPrimitive = srcPrimitive;
}

inline void DirectEnergySampleOutput::setObservationRay(const Ray& observationRay)
{
	m_observationRay = observationRay;
}

inline void DirectEnergySampleOutput::invalidate()
{
	// Must cuase `DirectEnergySampleOutput::operator bool ()` method to evaluate to `false`
	m_pdfW = 0;

	PH_ASSERT(!(*this));
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
	return m_pdfW;
}

inline const Primitive* DirectEnergySampleOutput::getSrcPrimitive() const
{
	PH_ASSERT(*this);
	return m_srcPrimitive;
}

inline const Ray& DirectEnergySampleOutput::getObservationRay() const
{
	PH_ASSERT(*this);
	return m_observationRay;
}

inline DirectEnergySampleOutput::operator bool () const
{
	return m_pdfW > 0;
}

}// end namespace ph
