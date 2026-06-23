#pragma once

#include "Engine/Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfEvalQuery.h"

namespace ph
{

inline BsdfSampleInput::BsdfSampleInput() = default;

inline BsdfSampleInput::BsdfSampleInput(
	const SurfaceHit& X,
	const math::Vector3R& V)

	: m_X(X)
	, m_V(V)
{
	PH_ASSERT(!X.getReason().hasExactly(ESurfaceHitReason::Invalid));
	PH_ASSERT_IN_RANGE(V.lengthSquared(), 0.9_r, 1.1_r);

#if PH_DEBUG
	m_hasSet = true;
#endif
}

inline BsdfSampleQuery::BsdfSampleQuery() = default;

inline BsdfSampleQuery::BsdfSampleQuery(BsdfQueryContext context)
	: BsdfSampleQuery()
{
	this->context = std::move(context);

	// (rest of the fields are initialized via setters)
}

inline BsdfSampleQuery::BsdfSampleQuery(
	BsdfQueryContext context,
	const SurfaceHit& X,
	const math::Vector3R& V)

	: context(std::move(context))
	, inputs(X, V)
{}

inline void BsdfSampleInput::set(const BsdfEvalInput& evalInput)
{
	set(
		evalInput.getX(),
		evalInput.getV());
}

}// end namespace ph
