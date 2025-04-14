#pragma once

#include "Engine/Math/TVector3.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Core/SurfaceHit.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

namespace ph
{

class MediumDistanceSampleInput final
{
public:
	void set(const SurfaceHit& X, const math::Vector3R& L, const real maxDist);

	const SurfaceHit& getX() const;
	const math::Vector3R& getL() const;
	real getMaxDist() const;

private:
	SurfaceHit     m_X;
	math::Vector3R m_L;
	real           m_maxDist;
#if PH_DEBUG
	bool           m_hasSet{false};
#endif
};

class MediumDistanceSampleOutput final
{
public:
	void setDist(real dist);

	void setPdfAppliedWeight(
		const math::Spectrum& pdfAppliedWeight,
		bool inferMeasurability = true);

	real getDist() const;
	const math::Spectrum& getPdfAppliedWeight() const;

	operator bool () const;

private:
	math::Spectrum m_pdfAppliedWeight{0};
	real           m_dist{0};
	bool           m_isMeasurable{false};
};

class MediumDistanceSampleQuery final
{
public:
	using Input  = MediumDistanceSampleInput;
	using Output = MediumDistanceSampleOutput;

	class Output final
	{
	public:
		real dist;
		math::Spectrum pdfAppliedWeight;
	};

	inline bool isMaxDistReached() const
	{
		return outputs.dist >= inputs.maxDist;
	}

public:
	Input  inputs;
	Output outputs;
};

inline void MediumDistanceSampleInput::set(
	const SurfaceHit&     X, 
	const math::Vector3R& L, 
	const real            maxDist)
{
	// Not querying from uninitialized surface hit
	PH_ASSERT(!X.getReason().hasExactly(ESurfaceHitReason::Invalid));

	PH_ASSERT_IN_RANGE(L.lengthSquared(), 0.9_r, 1.1_r);

	m_X       = X;
	m_L       = L;
	m_maxDist = maxDist;

#if PH_DEBUG
	m_hasSet = true;
#endif
}

inline const SurfaceHit& MediumDistanceSampleInput::getX() const
{
	PH_ASSERT(m_hasSet);

	return m_X;
}

inline const math::Vector3R& MediumDistanceSampleInput::getL() const
{
	PH_ASSERT(m_hasSet);

	return m_L;
}

inline real MediumDistanceSampleInput::getMaxDist() const
{
	PH_ASSERT(m_hasSet);

	return m_maxDist;
}

inline void MediumDistanceSampleOutput::setDist(real dist)
{
	PH_ASSERT_GE(dist, 0);

	m_dist = dist;
}

inline void MediumDistanceSampleOutput::setPdfAppliedWeight(
	const math::Spectrum& pdfAppliedWeight,
	bool inferMeasurability)
{
	m_pdfAppliedWeight = pdfAppliedWeight;

	if(inferMeasurability)
	{
		m_isMeasurable = pdfAppliedWeight.isMeasurable();
	}
}

inline real MediumDistanceSampleInput::getDist() const
{
	PH_ASSERT(*this);

	return m_dist;
}
inline const math::Spectrum& MediumDistanceSampleInput::getPdfAppliedWeight() const
{
	PH_ASSERT(*this);

	return m_pdfAppliedWeight;
}
inline const math::Spectrum& MediumDistanceSampleInput::getPdfAppliedWeight() const;

inline MediumDistanceSampleInput::operator bool() const;

}// end namespace ph
