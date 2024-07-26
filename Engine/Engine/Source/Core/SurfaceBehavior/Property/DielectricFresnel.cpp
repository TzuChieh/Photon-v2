#include "Core/SurfaceBehavior/Property/DielectricFresnel.h"
#include "Math/math.h"

#include <Common/assertion.h>

#include <cmath>
#include <algorithm>
#include <utility>

namespace ph
{

DielectricFresnel::DielectricFresnel(const real iorOuter, const real iorInner)
	: m_iorOuter(iorOuter)
	, m_iorInner(iorInner)
{}

std::optional<math::Vector3R> DielectricFresnel::calcRefractDir(
	const math::Vector3R& I,
	const math::Vector3R& N) const
{
	PH_ASSERT_IN_RANGE(I.lengthSquared(), 0.9_r, 1.1_r);
	PH_ASSERT_IN_RANGE(N.lengthSquared(), 0.9_r, 1.1_r);

	const real IoN      = I.dot(N);
	const real signIoN  = IoN < 0.0_r ? -1.0_r : 1.0_r;
	const real iorRatio = signIoN < 0.0_r ? m_iorInner / m_iorOuter : m_iorOuter / m_iorInner;
	const real sqrValue = 1.0_r - iorRatio * iorRatio * (1.0_r - IoN * IoN);

	// TIR
	if(sqrValue <= 0.0_r)
	{
		return std::nullopt;
	}

	const real Nfactor = iorRatio * IoN - signIoN * std::sqrt(sqrValue);
	const real Ifactor = -iorRatio;
	const math::Vector3R refractDir = N.mul(Nfactor).add(I.mul(Ifactor)).normalize();
	PH_ASSERT_IN_RANGE(refractDir.lengthSquared(), 0.9_r, 1.1_r);

	return refractDir;
}

std::optional<real> DielectricFresnel::calcRefractCos(
	const math::Vector3R& I,
	const math::Vector3R& N) const
{
	PH_ASSERT_IN_RANGE(I.lengthSquared(), 0.9_r, 1.1_r);
	PH_ASSERT_IN_RANGE(N.lengthSquared(), 0.9_r, 1.1_r);

	const real IoN = I.dot(N);

	real etaI = m_iorOuter;
	real etaT = m_iorInner;
	if(IoN < 0.0_r)
	{
		std::swap(etaI, etaT);
	}

	// Squaring Snell's law to find cosine values more efficiently
	const real cosT2 = 1.0_r - math::squared(etaI / etaT) * (1.0_r - IoN * IoN);

	// TIR
	if(cosT2 <= 0.0_r)
	{
		return std::nullopt;
	}

	return std::copysign(std::sqrt(cosT2), -IoN);
}

}// end namespace ph
