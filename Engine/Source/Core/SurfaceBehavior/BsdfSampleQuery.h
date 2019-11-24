#pragma once

#include "Core/SurfaceBehavior/bsdf_query_fwd.h"
#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/SurfaceBehavior/BsdfQueryContext.h"

#include <utility>

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
	math::Vector3R   L;
	SpectralStrength pdfAppliedBsdf;

	// Tells whether this sample has non-zero and sane contribution. All 
	// sample data should be usable if true is returned; otherwise, zero
	// contribution is implied, and sample data is undefined.
	bool isMeasurable() const;

	void setMeasurability(bool measurability);

private:
	bool m_isMeasurable{false};
};

class BsdfSampleQuery final
{
public:
	using Input  = BsdfSampleInput;
	using Output = BsdfSampleOutput;

	BsdfQueryContext context;
	Input            inputs;
	Output           outputs;

	BsdfSampleQuery();
	explicit BsdfSampleQuery(BsdfQueryContext context);
};

// In-header Implementations:

inline BsdfSampleQuery::BsdfSampleQuery() : 
	BsdfSampleQuery(BsdfQueryContext())
{}

inline BsdfSampleQuery::BsdfSampleQuery(BsdfQueryContext context) :
	context(std::move(context))
	// rest of the fields are initialized via setters
{}

inline void BsdfSampleInput::set(
	const SurfaceHit&     X, 
	const math::Vector3R& V)
{
	this->X = X;
	this->V = V;
}

inline bool BsdfSampleOutput::isMeasurable() const
{
	return m_isMeasurable;
}

inline void BsdfSampleOutput::setMeasurability(const bool measurability)
{
	m_isMeasurable = measurability;
}

}// end namespace ph
