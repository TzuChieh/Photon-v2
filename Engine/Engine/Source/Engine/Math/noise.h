#pragma once

#include "Engine/Math/math_fwd.h"

#include <Common/primitive_type.h>

namespace ph::math::noise
{

/*! @brief Evaluate raw signed Perlin gradient noise.
Values have an average near zero. Each dimensional overload has a different amplitude range;
callers needing comparable ranges must apply dimension-specific normalization. Large coordinate
magnitudes lose fractional precision; callers should range-reduce coordinates with a period
appropriate for their use case.
@pre Each coordinate is finite and its floored lattice coordinate is in `[-2^31, 2^31)`.
*/
///@{
float32 perlin(float32 position);
float32 perlin(const Vector2F& position);
float32 perlin(const Vector3F& position);
float32 perlin(const Vector4F& position);
///@}

}// end namespace ph::math::noise

#include "Engine/Math/noise.ipp"
