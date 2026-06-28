#pragma once

#include "Engine/Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfSampleQuery.h"

namespace ph
{

inline BsdfEvalInput::BsdfEvalInput() = default;

inline BsdfEvalInput::BsdfEvalInput(
	const SurfaceHit& X,
	const math::Vector3R& L,
	const math::Vector3R& V)

	: m_X(X)
	, m_L(L)
	, m_V(V)
{
	PH_ASSERT(!X.getReason().hasExactly(ESurfaceHitReason::Invalid));
	PH_ASSERT_IN_RANGE(L.lengthSquared(), 0.9_r, 1.1_r);
	PH_ASSERT_IN_RANGE(V.lengthSquared(), 0.9_r, 1.1_r);

#if PH_DEBUG
	m_hasSet = true;
#endif
}

inline BsdfEvalInput::BsdfEvalInput(const BsdfSampleInput& sampleInput, const BsdfSampleOutput& sampleOutput)
	: BsdfEvalInput(
		sampleInput.getX(),
		sampleOutput.getL(),
		sampleInput.getV())
{}

inline BsdfEvalQuery::BsdfEvalQuery() = default;

inline BsdfEvalQuery::BsdfEvalQuery(
	BsdfQueryContext context,
	const SurfaceHit& X,
	const math::Vector3R& L,
	const math::Vector3R& V)

	: context(std::move(context))
	, inputs(X, L, V)
{}

inline BsdfEvalQuery::BsdfEvalQuery(
	BsdfQueryContext context,
	const BsdfSampleInput& sampleInput,
	const BsdfSampleOutput& sampleOutput)

	: context(std::move(context))
	, inputs(sampleInput, sampleOutput)
{}

inline void BsdfEvalInput::set(const BsdfSampleQuery& sample)
{
	set(sample.inputs, sample.outputs);
}

inline void BsdfEvalInput::set(const BsdfSampleInput& sampleInput, const BsdfSampleOutput& sampleOutput)
{
	set(
		sampleInput.getX(),
		sampleOutput.getL(),
		sampleInput.getV());
}

}// end namespace ph
