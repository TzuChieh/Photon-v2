#pragma once

#include "Math/TVector3.h"
#include "Core/Ray.h"
#include "Core/Quantity/Time.h"
#include "Math/Color/Spectrum.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

namespace ph
{

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

class EnergyEmissionSampleOutput final
{
public:
	void setPdf(real pdfA, real pdfW);
	void setEmittedRay(const Ray& emittedRay);
	void setEmittedEnergy(const math::Spectrum& emittedEnergy);
	void invalidate();

	real getPdfA() const;
	real getPdfW() const;
	const Ray& getEmittedRay() const;
	const math::Spectrum& getEmittedEnergy() const;

	/*! @brief Checks the validity of the sample.
	@return `true` if the sample is valid. `false` if the sample failed. A failed sample cannot be
	used. A typical way to fail is that sampling is not even supported/possible.
	*/
	operator bool () const;

private:
	real m_pdfA{0};
	real m_pdfW{0};
	Ray m_emittedRay{};
	math::Spectrum m_emittedEnergy{0};
};

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

inline void EnergyEmissionSampleOutput::setPdf(const real pdfA, const real pdfW)
{
	m_pdfA = pdfA;
	m_pdfW = pdfW;
}

inline void EnergyEmissionSampleOutput::setEmittedRay(const Ray& emittedRay)
{
	m_emittedRay = emittedRay;
}

inline void EnergyEmissionSampleOutput::setEmittedEnergy(const math::Spectrum& emittedEnergy)
{
	m_emittedEnergy = emittedEnergy;
}

inline void EnergyEmissionSampleOutput::invalidate()
{
	// Must cuase `EnergyEmissionSampleOutput::operator bool ()` method to evaluate to `false`
	m_pdfA = 0;
	m_pdfW = 0;

	PH_ASSERT(!(*this));
}

inline real EnergyEmissionSampleOutput::getPdfA() const
{
	PH_ASSERT(*this);
	return m_pdfA;
}

inline real EnergyEmissionSampleOutput::getPdfW() const
{
	PH_ASSERT(*this);
	return m_pdfW;
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
	return m_pdfA > 0 && m_pdfW > 0;
}

}// end namespace ph
