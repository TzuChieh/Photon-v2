#pragma once

#include "Core/SurfaceBehavior/bsdf_query_fwd.h"
#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"
#include "Math/Color/Spectrum.h"
#include "Core/SurfaceBehavior/BsdfQueryContext.h"

#include <Common/assertion.h>

#include <utility>

// TODO: provide default ctors for these classes?

namespace ph
{

class BsdfSampleInput final
{
public:
	SurfaceHit     X;
	math::Vector3R V;

	void set(const BsdfEvalQuery& eval);

	void set(
		const SurfaceHit&     X, 
		const math::Vector3R& V); 
};

class BsdfSampleOutput final
{
public:
	math::Vector3R L;
	math::Spectrum pdfAppliedBsdf;

	/*! @brief Tells whether this sample has non-zero and sane contribution.
	All sampled data should be usable if true is returned; otherwise, zero contribution is implied,
	and sampled data is undefined.
	*/
	bool isMeasurable() const;

	/*! @brief Set measurability directly.
	*/
	void setMeasurability(bool measurability);

	/*! @brief Set measurability based on a reference spectrum.
	*/
	void setMeasurability(const math::Spectrum& reference);

private:
	bool m_isMeasurable = false;
};

class BsdfSampleQuery final
{
public:
	using Input  = BsdfSampleInput;
	using Output = BsdfSampleOutput;

	BsdfQueryContext context = BsdfQueryContext{};
	Input            inputs;
	Output           outputs;

	BsdfSampleQuery() = default;
	explicit BsdfSampleQuery(BsdfQueryContext context);
};

// In-header Implementations:

inline BsdfSampleQuery::BsdfSampleQuery(BsdfQueryContext context)
	: BsdfSampleQuery()
{
	this->context = std::move(context);

	// rest of the fields are initialized via setters
}

inline void BsdfSampleInput::set(
	const SurfaceHit&     X, 
	const math::Vector3R& V)
{
	this->X = X;
	this->V = V;
}

inline bool BsdfSampleOutput::isMeasurable() const
{
#if PH_DEBUG
	// When a sample report being measurable, it must be non-zero and not some crazy values
	if(m_isMeasurable)
	{
		PH_ASSERT(!pdfAppliedBsdf.isZero());
		PH_ASSERT_MSG(pdfAppliedBsdf.isFinite(), pdfAppliedBsdf.toString());
		PH_ASSERT_MSG(0.95_r < L.length() && L.length() < 1.05_r, L.toString());
	}
#endif

	return m_isMeasurable;
}

inline void BsdfSampleOutput::setMeasurability(const bool measurability)
{
	m_isMeasurable = measurability;
}

inline void BsdfSampleOutput::setMeasurability(const math::Spectrum& reference)
{
	setMeasurability(!reference.isZero() && reference.isFinite());
}

}// end namespace ph
