#pragma once

#include "Core/SurfaceBehavior/bsdf_query_fwd.h"
#include "Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"
#include "Math/Color/Spectrum.h"
#include "Core/SurfaceBehavior/surface_optics_fwd.h"

#include <Common/assertion.h>

#include <utility>

namespace ph
{

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

/*!
@note It is an error to get output data if `isMeasurable()` returns `false`.
*/
class BsdfEvalOutput
{
public:
	/*!
	@param bsdf The evaluated BSDF.
	@param inferMeasurability Whether to determine measurability from the supplied data.
	*/
	void setBsdf(
		const math::Spectrum& bsdf, 
		bool inferMeasurability = true);

	/*!
	@return Get the evaluated BSDF. Guaranteed to be finite.
	*/
	const math::Spectrum& getBsdf() const;

	/*! @brief Tells whether this evaluation has potential to contribute.
	All evaluated data should be usable if true is returned; otherwise, zero contribution is implied,
	and evaluated data is undefined. This method is also an efficient way to decide whether the BSDF
	has sane value (compared to manually testing its value).
	@note Measurability has nothing to do with whether the sampled value is 0 or not.
	*/
	bool isMeasurable() const;

	/*! @brief Set measurability directly.
	*/
	void setMeasurability(bool measurability);

	/*! @brief Set measurability based on a reference spectrum.
	*/
	void setMeasurability(const math::Spectrum& reference);

private:
	math::Spectrum m_bsdf{0};
	bool m_isMeasurable{false};
};

class BsdfEvalQuery final
{
public:
	using Input  = BsdfEvalInput;
	using Output = BsdfEvalOutput;
	
	BsdfQueryContext context = BsdfQueryContext{};
	Input            inputs;
	Output           outputs;

	BsdfEvalQuery() = default;
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
	const bool inferMeasurabilityFromThis)
{
	m_bsdf = bsdf;

	if(inferMeasurabilityFromThis)
	{
		setMeasurability(bsdf);
	}
}

inline const math::Spectrum& BsdfEvalOutput::getBsdf() const
{
	// When an evaluation report being measurable, it must not be some crazy values
	PH_ASSERT(m_isMeasurable);
	PH_ASSERT_MSG(m_bsdf.isFinite(), m_bsdf.toString());

	return m_bsdf;
}

inline bool BsdfEvalOutput::isMeasurable() const
{
	return m_isMeasurable;
}

inline void BsdfEvalOutput::setMeasurability(const bool measurability)
{
	m_isMeasurable = measurability;
}

inline void BsdfEvalOutput::setMeasurability(const math::Spectrum& reference)
{
	setMeasurability(reference.isFinite());
}

}// end namespace ph
