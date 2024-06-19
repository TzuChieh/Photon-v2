#pragma once

#include "Math/TVector3.h"
#include "Core/Ray.h"
#include "Core/Quantity/Time.h"
#include "Core/LTA/PDF.h"
#include "Math/Color/Spectrum.h"

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
*/
class EnergyEmissionSampleOutput final
{
public:
	void setPdf(const lta::PDF& pdfPos, const lta::PDF& pdfDir);
	void setEmittedRay(const Ray& emittedRay);
	void setEmittedEnergy(const math::Spectrum& emittedEnergy);

	real getPdfA() const;
	real getPdfW() const;
	const lta::PDF& getPdfPos() const;
	const lta::PDF& getPdfDir() const;
	const Ray& getEmittedRay() const;
	const math::Spectrum& getEmittedEnergy() const;

	/*! @brief Checks the validity of the sample.
	@return `true` if the sample is valid. `false` if the sample failed. A failed sample cannot be
	used. A typical way to fail is that sampling is not even supported/possible.
	*/
	operator bool () const;

private:
	lta::PDF       m_pdfPos{};
	lta::PDF       m_pdfDir{};
	Ray            m_emittedRay{};
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

inline void EnergyEmissionSampleOutput::setPdf(const lta::PDF& pdfPos, const lta::PDF& pdfDir)
{
	m_pdfPos = pdfPos;
	m_pdfDir = pdfDir;
}

inline void EnergyEmissionSampleOutput::setEmittedRay(const Ray& emittedRay)
{
	m_emittedRay = emittedRay;
}

inline void EnergyEmissionSampleOutput::setEmittedEnergy(const math::Spectrum& emittedEnergy)
{
	m_emittedEnergy = emittedEnergy;
}

inline real EnergyEmissionSampleOutput::getPdfA() const
{
	PH_ASSERT(*this);
	return m_pdfPos.getPdfA();
}

inline real EnergyEmissionSampleOutput::getPdfW() const
{
	PH_ASSERT(*this);
	return m_pdfDir.getPdfW();
}

inline const lta::PDF& EnergyEmissionSampleOutput::getPdfPos() const
{
	PH_ASSERT(*this);
	return m_pdfPos;
}

inline const lta::PDF& EnergyEmissionSampleOutput::getPdfDir() const
{
	PH_ASSERT(*this);
	return m_pdfDir;
}

inline const Ray& EnergyEmissionSampleOutput::getEmittedRay() const
{
	PH_ASSERT(*this);
	return m_emittedRay;
}

inline const math::Spectrum& EnergyEmissionSampleOutput::getEmittedEnergy() const
{
	PH_ASSERT(*this);
	return m_emittedEnergy;
}

inline EnergyEmissionSampleOutput::operator bool () const
{
	return m_pdfPos && m_pdfDir;
}

}// end namespace ph
