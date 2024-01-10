#pragma once

#include "Core/SurfaceBehavior/bsdf_query_fwd.h"
#include "Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"

namespace ph
{

class BsdfPdfInput final
{
public:
	SurfaceHit     X;
	math::Vector3R L;
	math::Vector3R V;

	void set(const BsdfEvalQuery& eval);
	void set(const BsdfSampleQuery& sample);
	void set(const BsdfSampleInput& sampleInput, const BsdfSampleOutput& sampleOutput);

	void set(
		const SurfaceHit&     X, 
		const math::Vector3R& L,
		const math::Vector3R& V);
};

class BsdfPdfOutput final
{
public:
	real sampleDirPdfW = 0.0_r;
};

class BsdfPdfQuery final
{
public:
	using Input  = BsdfPdfInput;
	using Output = BsdfPdfOutput;

	BsdfQueryContext context = BsdfQueryContext{};
	Input            inputs;
	Output           outputs;

	BsdfPdfQuery() = default;
	explicit BsdfPdfQuery(BsdfQueryContext context);
};

// In-header Implementations:

inline BsdfPdfQuery::BsdfPdfQuery(BsdfQueryContext context)
	: BsdfPdfQuery()
{
	this->context = std::move(context);

	// rest of the fields are initialized via setters
}

inline void BsdfPdfInput::set(
	const SurfaceHit&     X,
	const math::Vector3R& L,
	const math::Vector3R& V)
{
	this->X = X;
	this->L = L;
	this->V = V;
}

}// end namespace ph
