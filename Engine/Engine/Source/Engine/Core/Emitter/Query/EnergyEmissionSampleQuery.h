#pragma once

#include "Engine/Math/TVector3.h"
#include "Engine/Core/HitEvent.h"
#include "Engine/Core/Intersection/Query/PrimitivePosSampleQuery.h"
#include "Engine/Core/Ray.h"
#include "Engine/Core/Quantity/Time.h"
#include "Engine/Core/LTA/PDF.h"
#include "Engine/Math/Color/Spectrum.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

namespace ph
{

/*! @brief Input for `EnergyEmissionSampleQuery`.
*/
class EnergyEmissionSampleInput final
{
public:
	void set(const Time& time);

	const Time& getTime() const;

private:
	Time m_time;
#if PH_DEBUG
	bool m_hasSet{false};
#endif
};

/*! @brief Output for `EnergyEmissionSampleQuery`.
@note Output data is available only when `isContributable()` returns true.
*/
class EnergyEmissionSampleOutput final
{
public:
	/*! @brief Set both required PDFs.
	*/
	void setPdf(const lta::PDF& pdfPos, const lta::PDF& pdfDir);

	void setHitEvent(const HitEvent& event);

	/*! @brief Copy a position sample as a `SampledPosDir` event at the emitted ray's minimum t.
	@param posSample A valid position sample.
	@param emittedRay Outgoing ray originating at the sampled position, with the same sample time.
	*/
	void setHitEvent(const PrimitivePosSampleOutput& posSample, const Ray& emittedRay);

	void setEmittedEnergy(const math::Spectrum& emittedEnergy);

	bool isContributable() const;
	real getPdfA() const;
	real getPdfW() const;
	const lta::PDF& getPdfPos() const;
	const lta::PDF& getPdfDir() const;
	const Ray& getEmittedRay() const;
	const HitEvent& getHitEvent() const;
	const math::Spectrum& getEmittedEnergy() const;

	/*! @brief Access the event directly.
	*/
	HitEvent& event();

	/*! @brief Convenient method for `isContributable()`.
	@return `true` if the sample is contributable. `false` if the sample failed. A failed sample cannot be
	used. A typical way to fail is that sampling is not even supported/possible.
	*/
	operator bool () const;

private:
	lta::PDF       m_pdfPos{};
	lta::PDF       m_pdfDir{};
	HitEvent       m_hitEvent;
	math::Spectrum m_emittedEnergy{0};
};

/*! @brief Information for generating a sample for energy emission.
*/
class EnergyEmissionSampleQuery final
{
public:
	using Input  = EnergyEmissionSampleInput;
	using Output = EnergyEmissionSampleOutput;

	Input  inputs;
	Output outputs;

	EnergyEmissionSampleQuery() = default;
};

// In-header Implementations:

inline void EnergyEmissionSampleInput::set(
	const Time& time)
{
	m_time = time;

#if PH_DEBUG
	m_hasSet = true;
#endif
}

inline const Time& EnergyEmissionSampleInput::getTime() const
{
	PH_ASSERT(m_hasSet);
	return m_time;
}

inline bool EnergyEmissionSampleOutput::isContributable() const
{
	PH_ASSERT(!m_pdfPos || (m_pdfPos && m_pdfDir && m_emittedEnergy.isFinite()));
	return m_pdfPos;
}

inline void EnergyEmissionSampleOutput::setPdf(const lta::PDF& pdfPos, const lta::PDF& pdfDir)
{
	// We use position PDF to determine whether both PDFs are valid
	m_pdfPos = pdfPos && pdfDir ? pdfPos : lta::PDF{};

	m_pdfDir = pdfDir;
}

inline void EnergyEmissionSampleOutput::setHitEvent(const HitEvent& event)
{
	m_hitEvent = event;
}

inline void EnergyEmissionSampleOutput::setHitEvent(
	const PrimitivePosSampleOutput& posSample,
	const Ray& emittedRay)
{
	PH_ASSERT(emittedRay.getOrigin() == posSample.getPos());
	PH_ASSERT(emittedRay.getTime() == posSample.getObservationRay().getTime());

	m_hitEvent.setRay(emittedRay);
	m_hitEvent.copyHitAndKeepRay(posSample.getHitEvent(), emittedRay.getMinT());
	m_hitEvent.setReason(ESurfaceHitReason::SampledPosDir);
}

inline void EnergyEmissionSampleOutput::setEmittedEnergy(const math::Spectrum& emittedEnergy)
{
	m_emittedEnergy = emittedEnergy;
}

inline real EnergyEmissionSampleOutput::getPdfA() const
{
	return getPdfPos().getPdfA();
}

inline real EnergyEmissionSampleOutput::getPdfW() const
{
	return getPdfDir().getPdfW();
}

inline const lta::PDF& EnergyEmissionSampleOutput::getPdfPos() const
{
	PH_ASSERT(isContributable());
	return m_pdfPos;
}

inline const lta::PDF& EnergyEmissionSampleOutput::getPdfDir() const
{
	PH_ASSERT(isContributable());
	return m_pdfDir;
}

inline const Ray& EnergyEmissionSampleOutput::getEmittedRay() const
{
	return getHitEvent().getRay();
}

inline const math::Spectrum& EnergyEmissionSampleOutput::getEmittedEnergy() const
{
	PH_ASSERT(isContributable());
	return m_emittedEnergy;
}

inline const HitEvent& EnergyEmissionSampleOutput::getHitEvent() const
{
	PH_ASSERT(isContributable());
	return m_hitEvent;
}

inline HitEvent& EnergyEmissionSampleOutput::event()
{
	return m_hitEvent;
}

inline EnergyEmissionSampleOutput::operator bool () const
{
	return isContributable();
}

}// end namespace ph
