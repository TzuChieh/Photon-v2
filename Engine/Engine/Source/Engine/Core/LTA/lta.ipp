#pragma once

#include "Engine/Core/LTA/lta.h"

namespace ph::lta
{

inline real pdfA_to_pdfW(
	const real pdfA,
	const math::Vector3R& dAPosToTargetPos,
	const math::Vector3R& dANormal)
{
	const real distSquared = dAPosToTargetPos.lengthSquared();
	const real signedPdfW = pdfA / dAPosToTargetPos.normalize().dot(dANormal) * distSquared;
	return std::isfinite(signedPdfW) ? std::abs(signedPdfW) : 0.0_r;
}

}// end namespace ph::lta
