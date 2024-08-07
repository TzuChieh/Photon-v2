#pragma once

#include "Math/math.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cmath>

namespace ph
{

namespace lbconv
{

inline real alpha_to_variance(const real alpha)
{
	const real clampedAlpha = math::clamp(alpha, 0.0_r, 0.9999_r);
	const real alpha1p1 = std::pow(clampedAlpha, 1.1_r);
	return alpha1p1 / (1.0_r - alpha1p1);
}

inline real variance_to_alpha(const real variance)
{
	return std::pow(variance / (1.0_r + variance), 1.0_r / 1.1_r);
}

inline real g_to_variance(const real g)
{
	PH_ASSERT(0.0_r < g && g <= 1.0_r);

	return std::pow((1.0_r - g) / g, 0.8_r) / (1.0_r + g);
}

}// end namespace conversions

}// end namespace ph