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
	SurfaceHit     X;
	math::Vector3R L;
	math::Vector3R V;

	void set(const BsdfSampleQuery& sample);
	void set(const BsdfSampleInput& sampleInput, const BsdfSampleOutput& sampleOutput);

	void set(
		const SurfaceHit&     X, 
		const math::Vector3R& L,
		const math::Vector3R& V);
};

class BsdfEvalOutput
{
public:
	math::Spectrum bsdf;

	/*! @brief Tells whether this evaluation has non-zero and sane contribution.
	All evaluated data should be usable if true is returned; otherwise, zero contribution is implied,
	and evaluated data is undefined.
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

	// rest of the fields are initialized via setters
}

inline void BsdfEvalInput::set(
	const SurfaceHit&     X, 
	const math::Vector3R& L,
	const math::Vector3R& V)
{
	this->X = X;
	this->L = L;
	this->V = V;
}

inline bool BsdfEvalOutput::isMeasurable() const
{
#if PH_DEBUG
	// When an evaluation report being measurable, it must be non-zero and not some crazy values
	if(m_isMeasurable)
	{
		PH_ASSERT(!bsdf.isZero());
		PH_ASSERT_MSG(bsdf.isFinite(), bsdf.toString());
	}
#endif

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
