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
	STRICT,

	// Judging sidedness agreement solely on geometry normal.
	TRUST_GEOMETRY,
	
	// Judging sidedness agreement solely on shading normal.
	TRUST_SHADING
};

template<ESaPolicy POLICY>
class TSidednessAgreement
{
public:
	bool isSidednessAgreed(
		const SurfaceHit& X, 
		const Vector3R&   targetVector) const;

	bool isSameHemisphere(
		const SurfaceHit& X, 
		const Vector3R&   vecA, 
		const Vector3R&   vecB) const;

	bool isOppositeHemisphere(
		const SurfaceHit& X,
		const Vector3R&   vecA,
		const Vector3R&   vecB) const;

	void adjustForSidednessAgreement(
		SurfaceHit& X) const;

private:
	// TODO: variables for runtime usage
};

// In-header Implementations:

template<ESaPolicy POLICY>
inline bool TSidednessAgreement<POLICY>::isSidednessAgreed(
	const SurfaceHit& X,
	const Vector3R&   targetVector) const
{
	if constexpr(
		POLICY == ESaPolicy::DO_NOT_CARE    || 
		POLICY == ESaPolicy::TRUST_GEOMETRY ||
		POLICY == ESaPolicy::TRUST_SHADING)
	{
		// no agreement issue with single input vector with these policies
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
inline bool TSidednessAgreement<POLICY>::isSameHemisphere(
	const SurfaceHit& X,
	const Vector3R&   vecA,
	const Vector3R&   vecB) const
{
	if constexpr(POLICY == ESaPolicy::STRICT)
	{
		const Vector3R& N = X.getGeometryNormal();

		return isSidednessAgreed(X, vecA) &&     // Both vectors need to be strictly
		       isSidednessAgreed(X, vecB) &&     // agreed on sidedness.
		       vecA.dot(N) * vecB.dot(N) > 0.0_r;// Then testing hemisphere with either normal
		                                         // (the other normal would yield same sign)
	}
	else if constexpr(POLICY == ESaPolicy::TRUST_GEOMETRY)
	{
		const Vector3R& Ng = X.getGeometryNormal();

		return Ng.dot(vecA) * Ng.dot(vecB) > 0.0_r;
	}
	else if constexpr(
		POLICY == ESaPolicy::TRUST_SHADING || 
		POLICY == ESaPolicy::DO_NOT_CARE)
	{
		const Vector3R& Ns = X.getShadingNormal();

		return Ns.dot(vecA) * Ns.dot(vecB) > 0.0_r;
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
		return false;
	}
}

template<ESaPolicy POLICY>
inline bool TSidednessAgreement<POLICY>::isOppositeHemisphere(
	const SurfaceHit& X,
	const Vector3R&   vecA,
	const Vector3R&   vecB) const
{
	if constexpr(POLICY == ESaPolicy::STRICT)
	{
		const Vector3R& N = X.getGeometryNormal();

		return isSidednessAgreed(X, vecA) &&     // Both vectors need to be strictly
		       isSidednessAgreed(X, vecB) &&     // agreed on sidedness.
		       vecA.dot(N) * vecB.dot(N) < 0.0_r;// Then testing hemisphere with either normal
		                                         // (the other normal would yield same sign)
	}
	else if constexpr(POLICY == ESaPolicy::TRUST_GEOMETRY)
	{
		const Vector3R& Ng = X.getGeometryNormal();

		return Ng.dot(vecA) * Ng.dot(vecB) < 0.0_r;
	}
	else if constexpr(
		POLICY == ESaPolicy::TRUST_SHADING || 
		POLICY == ESaPolicy::DO_NOT_CARE)
	{
		const Vector3R& Ns = X.getShadingNormal();

		return Ns.dot(vecA) * Ns.dot(vecB) < 0.0_r;
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
		return false;
	}
}

template<ESaPolicy POLICY>
inline void TSidednessAgreement<POLICY>::adjustForSidednessAgreement(
	SurfaceHit& X) const
{
	// currently no adjustment
}

}// end namespace ph