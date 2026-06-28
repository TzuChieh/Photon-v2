#pragma once

#include "Engine/Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfSampleQuery.h"

namespace ph
{

inline BsdfPdfInput::BsdfPdfInput() = default;

inline BsdfPdfInput::BsdfPdfInput(
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

inline BsdfPdfInput::BsdfPdfInput(const BsdfEvalInput& evalInput)
	: BsdfPdfInput(
		evalInput.getX(),
		evalInput.getL(),
		evalInput.getV())
{}

inline BsdfPdfInput::BsdfPdfInput(const BsdfSampleInput& sampleInput, const BsdfSampleOutput& sampleOutput)
	: BsdfPdfInput(
		sampleInput.getX(),
		sampleOutput.getL(),
		sampleInput.getV())
{}

inline BsdfPdfInput::BsdfPdfInput(const BsdfSampleQuery& sample)
	: BsdfPdfInput(sample.inputs, sample.outputs)
{}

inline BsdfPdfQuery::BsdfPdfQuery() = default;

inline BsdfPdfQuery::BsdfPdfQuery(
	BsdfQueryContext context,
	const SurfaceHit& X,
	const math::Vector3R& L,
	const math::Vector3R& V)

	: context(std::move(context))
	, inputs(X, L, V)
{}

inline BsdfPdfQuery::BsdfPdfQuery(BsdfQueryContext context, const BsdfEvalInput& evalInput)
	: context(std::move(context))
	, inputs(evalInput)
{}

inline BsdfPdfQuery::BsdfPdfQuery(
	BsdfQueryContext context,
	const BsdfSampleInput& sampleInput,
	const BsdfSampleOutput& sampleOutput)

	: context(std::move(context))
	, inputs(sampleInput, sampleOutput)
{}

inline BsdfPdfQuery::BsdfPdfQuery(BsdfQueryContext context, const BsdfSampleQuery& sample)
	: BsdfPdfQuery(std::move(context), sample.inputs, sample.outputs)
{}

inline void BsdfPdfInput::set(const BsdfEvalInput& evalInput)
{
	set(
		evalInput.getX(),
		evalInput.getL(),
		evalInput.getV());
}

inline void BsdfPdfInput::set(const BsdfSampleQuery& sample)
{
	set(sample.inputs, sample.outputs);
}

inline void BsdfPdfInput::set(const BsdfSampleInput& sampleInput, const BsdfSampleOutput& sampleOutput)
{
	set(
		sampleInput.getX(),
		sampleOutput.getL(),
		sampleInput.getV());
}

}// end namespace ph
