#pragma once

#include "Engine/Math/TVector3.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Core/VolumeHit.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

namespace ph
{

class MediumDistanceSampleInput final
{
public:
	void set(const VolumeHit& X, const math::Vector3R& L, real maxDist);

	const VolumeHit& getX() const;
	const math::Vector3R& getL() const;
	real getMaxDist() const;

private:
	VolumeHit      m_X{};
	math::Vector3R m_L{0};
	real           m_maxDist{0};
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
		bool inferContributability = true);

	void setContributability(bool contributability);

	void setContributability(const math::Spectrum& reference);

	real getDist() const;
	const math::Spectrum& getPdfAppliedWeight() const;
	bool isContributable() const;

	/*! @brief Convenient method for `isContributable()`.
	*/
	operator bool() const;

private:
	math::Spectrum m_pdfAppliedWeight{0};
	real           m_dist{0};
	bool           m_isContributable{false};
};

class MediumDistanceSampleQuery final
{
public:
	using Input  = MediumDistanceSampleInput;
	using Output = MediumDistanceSampleOutput;

	Input  inputs;
	Output outputs;

	inline MediumDistanceSampleQuery() = default;

	bool isMaxDistReached() const;
};

inline void MediumDistanceSampleInput::set(
	const VolumeHit&      X,
	const math::Vector3R& L, 
	const real            maxDist)
{
	// Not querying from uninitialized surface hit
	PH_ASSERT(!X.getReason().hasExactly(EVolumeHitReason::Invalid));

	PH_ASSERT_IN_RANGE(L.lengthSquared(), 0.9_r, 1.1_r);

	m_X       = X;
	m_L       = L;
	m_maxDist = maxDist;

#if PH_DEBUG
	m_hasSet = true;
#endif
}

inline const VolumeHit& MediumDistanceSampleInput::getX() const
{
	return m_X;
}

inline const math::Vector3R& MediumDistanceSampleInput::getL() const
{
	return m_L;
}

inline real MediumDistanceSampleInput::getMaxDist() const
{
	return m_maxDist;
}

inline void MediumDistanceSampleOutput::setDist(real dist)
{
	PH_ASSERT_GE(dist, 0);

	m_dist = dist;
}

inline void MediumDistanceSampleOutput::setPdfAppliedWeight(
	const math::Spectrum& pdfAppliedWeight,
	bool inferContributability)
{
	m_pdfAppliedWeight = pdfAppliedWeight;

	if(inferContributability)
	{
		setContributability(pdfAppliedWeight);
	}
}

inline void MediumDistanceSampleOutput::setContributability(bool contributability)
{
	m_isContributable = contributability;
}

inline void MediumDistanceSampleOutput::setContributability(const math::Spectrum& reference)
{
	setContributability(reference.isFinite());
}

inline real MediumDistanceSampleOutput::getDist() const
{
	PH_ASSERT(isContributable());

	return m_dist;
}

inline const math::Spectrum& MediumDistanceSampleOutput::getPdfAppliedWeight() const
{
	// When a sample report being contributable, it must not be some crazy values
	PH_ASSERT(isContributable());
	PH_ASSERT_MSG(m_pdfAppliedWeight.isFinite(), m_pdfAppliedWeight.toString());

	return m_pdfAppliedWeight;
}

inline bool MediumDistanceSampleOutput::isContributable() const
{
	return m_isContributable;
}

inline MediumDistanceSampleOutput::operator bool() const
{
	return isContributable();
}

inline bool MediumDistanceSampleQuery::isMaxDistReached() const
{
	return outputs.getDist() >= inputs.getMaxDist();
}

}// end namespace ph
