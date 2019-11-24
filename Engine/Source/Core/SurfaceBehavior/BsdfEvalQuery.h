#pragma once

#include "Core/SurfaceBehavior/bsdf_query_fwd.h"
#include "Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/SurfaceBehavior/surface_optics_fwd.h"

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
	SpectralStrength bsdf;

	bool isGood() const;
};

class BsdfEvalQuery final
{
public:
	using Input  = BsdfEvalInput;
	using Output = BsdfEvalOutput;
	
	BsdfQueryContext context;
	Input            inputs;
	Output           outputs;

	BsdfEvalQuery();
	explicit BsdfEvalQuery(BsdfQueryContext context);
};

// In-header Implementations:

inline BsdfEvalQuery::BsdfEvalQuery() :
	BsdfEvalQuery(BsdfQueryContext())
{}

inline BsdfEvalQuery::BsdfEvalQuery(BsdfQueryContext context) :
	context(std::move(context))
	// rest of the fields are initialized via setters
{}

inline void BsdfEvalInput::set(
	const SurfaceHit&     X, 
	const math::Vector3R& L,
	const math::Vector3R& V)
{
	this->X = X;
	this->L = L;
	this->V = V;
}

inline bool BsdfEvalOutput::isGood() const
{
	return bsdf.isFinite();
}

}// end namespace ph
