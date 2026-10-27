#pragma once

#include "Engine/Math/TVector3.h"
#include "Engine/Core/HitEvent.h"
#include "Engine/Core/Ray.h"
#include "Engine/Core/Quantity/Time.h"
#include "Engine/Core/SurfaceHit.h"
#include "Engine/Core/LTA/PDF.h"
#include "Engine/Math/Color/Spectrum.h"

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
@note Output data is available only when `isContributable()` returns true.
*/
class DirectEnergySampleOutput final
{
public:
	void setEmitPos(const math::Vector3R& emitPos);
	void setEmittedEnergy(const math::Spectrum& emittedEnergy);

	/*! @brief Set the PDF. Invalid PDF makes this sample not contributable.
	*/
	void setPdf(const lta::PDF& pdf);

	void setSrcPrimitive(const Primitive* srcPrimitive);
	void setHitEvent(const HitEvent& event);
	
	bool isContributable() const;
	
	/*!
	@return The position that emits energy.
	*/
	const math::Vector3R& getEmitPos() const;

	/*! @brief The sampled emitted energy of. Does not contain any path weighting.
	*/
	const math::Spectrum& getEmittedEnergy() const;

	real getPdfW() const;
	const lta::PDF& getPdf() const;
	const Primitive& getSrcPrimitive() const;

	/*! @brief Get the ray from target position to sampled emitting position.
	If target position or emitting position is required, it is recommended to use
	`DirectEnergySampleInput::getTargetPos()` and `getEmitPos()`, respectively. Using the head and
	tail of the observation ray tends to have larger numerical error.
	*/
	const Ray& getObservationRay() const;
	
	const HitEvent& getHitEvent() const;

	/*! @brief Convenient method for `isContributable()`.
	@return `true` if the sample is contributable. `false` if the sample failed. A failed sample cannot be
	used. A typical way to fail is that sampling is not even supported/possible.
	*/
	operator bool () const;

private:
	math::Vector3R   m_emitPos{0};
	math::Spectrum   m_emittedEnergy{0};
	lta::PDF         m_pdf{};
	const Primitive* m_srcPrimitive{nullptr};
	HitEvent         m_hitEvent;
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

inline bool DirectEnergySampleOutput::isContributable() const
{
	PH_ASSERT(!m_pdf || (m_pdf && m_emittedEnergy.isFinite()));
	return m_pdf;
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

inline void DirectEnergySampleOutput::setHitEvent(const HitEvent& event)
{
	m_hitEvent = event;
}

inline const math::Vector3R& DirectEnergySampleOutput::getEmitPos() const
{
	PH_ASSERT(isContributable());
	return m_emitPos;
}

inline const math::Spectrum& DirectEnergySampleOutput::getEmittedEnergy() const
{
	PH_ASSERT(isContributable());
	return m_emittedEnergy;
}

inline real DirectEnergySampleOutput::getPdfW() const
{
	return getPdf().getPdfW();
}

inline const lta::PDF& DirectEnergySampleOutput::getPdf() const
{
	PH_ASSERT(isContributable());
	return m_pdf;
}

inline const Primitive& DirectEnergySampleOutput::getSrcPrimitive() const
{
	PH_ASSERT(isContributable());
	PH_ASSERT(m_srcPrimitive);
	return *m_srcPrimitive;
}

inline const Ray& DirectEnergySampleOutput::getObservationRay() const
{
	PH_ASSERT(isContributable());
	return getHitEvent().getRay();
}

inline const HitEvent& DirectEnergySampleOutput::getHitEvent() const
{
	PH_ASSERT(isContributable());
	return m_hitEvent;
}

inline DirectEnergySampleOutput::operator bool () const
{
	return isContributable();
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
