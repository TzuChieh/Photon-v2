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
It is an error to get output data if `isMeasurable()` returns `false`.
*/
class BsdfEvalOutput
{
public:
	void setBsdf(
		const math::Spectrum& bsdf, 
		bool inferMeasurabilityFromThis = true);

	/*!
	@return Get the evaluated BSDF.
	*/
	const math::Spectrum& getBsdf() const;

	/*! @brief Tells whether this evaluation has non-zero and sane contribution.
	All evaluated data should be usable if true is returned; otherwise, zero contribution is implied,
	and evaluated data is undefined. This method is an efficient way to decide whether the BSDF has
	potential to contribute.
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
	// When an evaluation report being measurable, it must be non-zero and not some crazy values
	PH_ASSERT(m_isMeasurable);
	PH_ASSERT(!m_bsdf.isZero());
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
	setMeasurability(!reference.isZero() && reference.isFinite());
}

}// end namespace ph
