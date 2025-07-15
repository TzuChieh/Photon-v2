#pragma once

#include "Engine/Core/SurfaceBehavior/bsdf_query_fwd.h"
#include "Engine/Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Core/SurfaceHit.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Core/SurfaceBehavior/surface_optics_fwd.h"

#include <Common/assertion.h>

#include <utility>

namespace ph
{

/*! @brief Input for `BsdfEvalQuery`.
*/
class BsdfEvalInput final
{
public:
	void set(const BsdfSampleQuery& sample);
	void set(const BsdfSampleInput& sampleInput, const BsdfSampleOutput& sampleOutput);

	void set(
		const SurfaceHit&     X, 
		const math::Vector3R& L,
		const math::Vector3R& V);

	const SurfaceHit& getX() const;
	const math::Vector3R& getL() const;
	const math::Vector3R& getV() const;

private:
	SurfaceHit     m_X;
	math::Vector3R m_L;
	math::Vector3R m_V;
#if PH_DEBUG
	bool           m_hasSet{false};
#endif
};

/*! @brief Output for `BsdfEvalQuery`.
@note It is an error to get output data if `isContributable()` returns `false`.
*/
class BsdfEvalOutput
{
public:
	/*!
	@param bsdf The evaluated BSDF.
	@param inferContributability Whether to determine contributability from the supplied data.
	*/
	void setBsdf(
		const math::Spectrum& bsdf, 
		bool inferContributability = true);

	/*!
	@return Get the evaluated BSDF. Guaranteed to be finite.
	*/
	const math::Spectrum& getBsdf() const;

	/*! @brief Tells whether this evaluation has potential to contribute.
	All evaluated data should be usable if true is returned; otherwise, zero contribution is implied,
	and evaluated data is undefined. This method is also an efficient way to decide whether the BSDF
	has sane value (compared to manually testing its value).
	@note Contributability has nothing to do with whether the sampled value is 0 or not.
	*/
	bool isContributable() const;

	/*! @brief Set contributability directly.
	*/
	void setContributability(bool contributability);

	/*! @brief Set contributability based on a reference spectrum.
	*/
	void setContributability(const math::Spectrum& reference);

	void add(const BsdfEvalOutput& other);

	/*! @brief Convenient method for `isContributable()`.
	*/
	operator bool () const;

private:
	math::Spectrum m_bsdf{0};
	bool m_isContributable{false};
};

/*! @brief Information for obtaining a sample value from BSDF.
*/
class BsdfEvalQuery final
{
public:
	using Input  = BsdfEvalInput;
	using Output = BsdfEvalOutput;
	
	BsdfQueryContext context = BsdfQueryContext{};
	Input            inputs;
	Output           outputs;

	inline BsdfEvalQuery() = default;
	explicit BsdfEvalQuery(BsdfQueryContext context);
};

// In-header Implementations:

inline BsdfEvalQuery::BsdfEvalQuery(BsdfQueryContext context)
	: BsdfEvalQuery()
{
	this->context = std::move(context);

	// (rest of the fields are initialized via setters)
}

inline void BsdfEvalInput::set(
	const SurfaceHit&     X, 
	const math::Vector3R& L,
	const math::Vector3R& V)
{
	// Not querying from uninitialized surface hit
	PH_ASSERT(!X.getReason().hasExactly(ESurfaceHitReason::Invalid));

	PH_ASSERT_IN_RANGE(L.lengthSquared(), 0.9_r, 1.1_r);
	PH_ASSERT_IN_RANGE(V.lengthSquared(), 0.9_r, 1.1_r);

	m_X = X;
	m_L = L;
	m_V = V;

#if PH_DEBUG
	m_hasSet = true;
#endif
}

inline const SurfaceHit& BsdfEvalInput::getX() const
{
	PH_ASSERT(m_hasSet);

	return m_X;
}

inline const math::Vector3R& BsdfEvalInput::getL() const
{
	PH_ASSERT(m_hasSet);

	return m_L;
}

inline const math::Vector3R& BsdfEvalInput::getV() const
{
	PH_ASSERT(m_hasSet);

	return m_V;
}

inline void BsdfEvalOutput::setBsdf(
	const math::Spectrum& bsdf,
	const bool inferContributabilityFromThis)
{
	m_bsdf = bsdf;

	if(inferContributabilityFromThis)
	{
		setContributability(bsdf);
	}
}

inline const math::Spectrum& BsdfEvalOutput::getBsdf() const
{
	// When an evaluation report being contributable, it must not be some crazy values
	PH_ASSERT(m_isContributable);
	PH_ASSERT_MSG(m_bsdf.isFinite(), m_bsdf.toString());

	return m_bsdf;
}

inline bool BsdfEvalOutput::isContributable() const
{
	return m_isContributable;
}

inline void BsdfEvalOutput::setContributability(const bool contributability)
{
	m_isContributable = contributability;
}

inline void BsdfEvalOutput::setContributability(const math::Spectrum& reference)
{
	setContributability(reference.isFinite());
}

inline void BsdfEvalOutput::add(const BsdfEvalOutput& other)
{
	m_isContributable = isContributable() || other.isContributable();

	if(other.isContributable())
	{
		m_bsdf += other.m_bsdf;
	}
}

inline BsdfEvalOutput::operator bool () const
{
	return isContributable();
}

}// end namespace ph
