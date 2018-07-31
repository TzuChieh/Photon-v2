#pragma once

#include "Core/SurfaceHit.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"

namespace ph
{

/*
	Options for dealing with situations where a vector is within the hemisphere
	defined by geometry normal but outside the one defined by shading normal or
	vice versa.
*/
enum class ESaPolicy
{
	// Perform any calculations without caring sidedness agreement.
	DO_NOT_CARE,

	// A vector must lies in hemispheres defined by geometry normal and shading
	// normal simultaneously.
	STRICT
};

template<ESaPolicy POLICY>
class TSidednessAgreement
{
public:
	bool isSidednessAgreed(
		const SurfaceHit& X, 
		const Vector3R&   targetVector) const;

	void adjustForSidednessAgreement(
		SurfaceHit& X) const;

private:
	// TODO: variables for runtime usage
};

// In-header Implementations:

template<ESaPolicy POLICY>
bool TSidednessAgreement<POLICY>::isSidednessAgreed(
	const SurfaceHit& X,
	const Vector3R&   targetVector) const
{
	if constexpr(POLICY == ESaPolicy::DO_NOT_CARE)
	{
		return true;
	}
	else if constexpr(POLICY == ESaPolicy::STRICT)
	{
		const Vector3R& Ng = X.getGeometryNormal();
		const Vector3R& Ns = X.getShadingNormal();

		return Ng.dot(targetVector) * Ns.dot(targetVector) > 0.0_r;
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
		return false;
	}
}

template<ESaPolicy POLICY>
void TSidednessAgreement<POLICY>::adjustForSidednessAgreement(
	SurfaceHit& X) const
{
	// currently no adjustment
}

}// end namespace ph