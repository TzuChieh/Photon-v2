#pragma once

#include "Core/SurfaceHit.h"
#include "Math/TVector3.h"

#include <Common/assertion.h>

namespace ph
{

/*!
Options for dealing with situations where a vector is within the hemisphere
defined by geometry normal but outside the one defined by shading normal or
vice versa.
*/
enum class ESidednessPolicy
{
	/*! Perform any calculations without caring sidedness agreement. */
	DoNotCare,

	/*! A vector must lies in hemispheres defined by geometry normal and shading normal simultaneously. */
	Strict,

	/*! Judging sidedness agreement solely on geometry normal. */
	TrustGeometry,
	
	/*! Judging sidedness agreement solely on shading normal. */
	TrustShading
};

class SidednessAgreement
{
public:
	SidednessAgreement();
	explicit SidednessAgreement(ESidednessPolicy policy);

	bool isSidednessAgreed(
		const SurfaceHit&     X, 
		const math::Vector3R& vec) const;

	bool isSameHemisphere(
		const SurfaceHit&     X, 
		const math::Vector3R& vecA,
		const math::Vector3R& vecB) const;

	bool isOppositeHemisphere(
		const SurfaceHit&     X,
		const math::Vector3R& vecA,
		const math::Vector3R& vecB) const;

	void adjustForSidednessAgreement(SurfaceHit& X) const;

private:
	ESidednessPolicy m_policy;
};

// In-header Implementations:

inline SidednessAgreement::SidednessAgreement() : 
	SidednessAgreement(ESidednessPolicy::DoNotCare)
{}

inline SidednessAgreement::SidednessAgreement(const ESidednessPolicy policy) :
	m_policy(policy)
{}

inline bool SidednessAgreement::isSidednessAgreed(
	const SurfaceHit&     X,
	const math::Vector3R& vec) const
{
	switch(m_policy)
	{
	case ESidednessPolicy::DoNotCare:
	case ESidednessPolicy::TrustGeometry:
	case ESidednessPolicy::TrustShading:
	{
		// no agreement issue with single input vector with these policies
		return true;
		break;
	}

	case ESidednessPolicy::Strict:
	{
		const math::Vector3R& Ng = X.getGeometryNormal();
		const math::Vector3R& Ns = X.getShadingNormal();

		return Ng.dot(vec) * Ns.dot(vec) > 0.0_r;
		break;
	}

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return false;
		break;
	}
}

inline bool SidednessAgreement::isSameHemisphere(
	const SurfaceHit&     X,
	const math::Vector3R& vecA,
	const math::Vector3R& vecB) const
{
	switch(m_policy)
	{
	case ESidednessPolicy::TrustGeometry:
	case ESidednessPolicy::DoNotCare:
	{
		const math::Vector3R& Ng = X.getGeometryNormal();

		return Ng.dot(vecA) * Ng.dot(vecB) > 0.0_r;
		break;
	}

	case ESidednessPolicy::TrustShading:
	{
		const math::Vector3R& Ns = X.getShadingNormal();

		return Ns.dot(vecA) * Ns.dot(vecB) > 0.0_r;
		break;
	}

	case ESidednessPolicy::Strict:
	{
		const math::Vector3R& N = X.getGeometryNormal();

		return isSidednessAgreed(X, vecA) &&     // Both vectors need to be strictly
		       isSidednessAgreed(X, vecB) &&     // agreed on sidedness.
		       vecA.dot(N) * vecB.dot(N) > 0.0_r;// Then testing hemisphere with either normal
		                                         // (the other normal would yield same sign)
		break;
	}

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return false;
		break;
	}
}

inline bool SidednessAgreement::isOppositeHemisphere(
	const SurfaceHit&     X,
	const math::Vector3R& vecA,
	const math::Vector3R& vecB) const
{
	switch(m_policy)
	{
	case ESidednessPolicy::TrustGeometry:
	case ESidednessPolicy::DoNotCare:
	{
		const math::Vector3R& Ng = X.getGeometryNormal();

		return Ng.dot(vecA) * Ng.dot(vecB) < 0.0_r;
		break;
	}

	case ESidednessPolicy::TrustShading:
	{
		const math::Vector3R& Ns = X.getShadingNormal();

		return Ns.dot(vecA) * Ns.dot(vecB) < 0.0_r;
		break;
	}

	case ESidednessPolicy::Strict:
	{
		const math::Vector3R& N = X.getGeometryNormal();

		return isSidednessAgreed(X, vecA) &&     // Both vectors need to be strictly
		       isSidednessAgreed(X, vecB) &&     // agreed on sidedness.
		       vecA.dot(N) * vecB.dot(N) < 0.0_r;// Then testing hemisphere with either normal
		                                         // (the other normal would yield same sign)
	}

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return false;
		break;
	}
}

inline void SidednessAgreement::adjustForSidednessAgreement(
	SurfaceHit& X) const
{
	// currently no adjustment
}

}// end namespace ph
