#pragma once

#include "Engine/Math/noise.h"
#include "Engine/Math/hash.h"
#include "Engine/Math/TVector2.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Math/TVector4.h"

#include <Common/assertion.h>

#include <cmath>
#include <limits>

namespace ph::math::noise
{

/*
- Reference implementation: Blender Cycles `src/kernel/svm/noise.h`, functions `fade`,
  `negate_if`, `grad1`, `grad2`, `grad3`, `grad4`, `bi_mix`, `tri_mix`, `quad_mix`,
  `perlin_1d`, `perlin_2d`, `perlin_3d`, and `perlin_4d`:
  https://github.com/blender/cycles/blob/97dbe6f57cdf4ede2d2b75ebdda507c8712edb7a/src/kernel/svm/noise.h#L16-L249
- Scalar interpolation reference: Blender Cycles `src/util/math_base.h`, function `mix`:
  https://github.com/blender/cycles/blob/97dbe6f57cdf4ede2d2b75ebdda507c8712edb7a/src/util/math_base.h#L403-L406
*/
namespace detail::perlin
{

// Perlin lattice coordinates use floor-relative fractions so negative inputs remain in [0, 1).
inline float32 floor_fraction(const float32 value, uint32& outCell)
{
	constexpr float32 minSignedCell          = static_cast<float32>(std::numeric_limits<int32>::min());
	constexpr float32 maxSignedCellExclusive = -minSignedCell;

	const auto floorValue = std::floor(value);
	PH_ASSERT_IN_RANGE(floorValue, minSignedCell, maxSignedCellExclusive);
	outCell = static_cast<uint32>(static_cast<int32>(floorValue));
	return value - floorValue;
}

inline float32 fade(const float32 value)
{
	return value * value * value * (value * (value * 6.0f - 15.0f) + 10.0f);
}

inline float32 negate_if(const float32 value, const bool shouldNegate)
{
	return shouldNegate ? -value : value;
}

inline float32 gradient_dot_1d(const uint32 hash, const float32 x)
{
	const uint32 h = hash & 15;
	const auto gradient = static_cast<float32>(1 + (h & 7));
	return negate_if(gradient, (h & 8) != 0) * x;
}

inline float32 gradient_dot_2d(const uint32 hash, const float32 x, const float32 y)
{
	const uint32 h = hash & 7;
	const float32 u = h < 4 ? x : y;
	const float32 v = 2.0f * (h < 4 ? y : x);
	return negate_if(u, (h & 1) != 0) + negate_if(v, (h & 2) != 0);
}

inline float32 gradient_dot_3d(
	const uint32 hash,
	const float32 x,
	const float32 y,
	const float32 z)
{
	const uint32 h = hash & 15;
	const float32 u = h < 8 ? x : y;
	const float32 vt = h == 12 || h == 14 ? x : z;
	const float32 v = h < 4 ? y : vt;
	return negate_if(u, (h & 1) != 0) + negate_if(v, (h & 2) != 0);
}

inline float32 gradient_dot_4d(
	const uint32 hash,
	const float32 x,
	const float32 y,
	const float32 z,
	const float32 w)
{
	const uint32 h = hash & 31;
	const float32 u = h < 24 ? x : y;
	const float32 v = h < 16 ? y : z;
	const float32 s = h < 8 ? z : w;
	return negate_if(u, (h & 1) != 0) +
	       negate_if(v, (h & 2) != 0) +
	       negate_if(s, (h & 4) != 0);
}

/*
Linear interpolation:

  a ●──────────────────*──────────────────● b
    0                  factor             1
*/
inline float32 mix(const float32 a, const float32 b, const float32 factor)
{
	return a + factor * (b - a);
}

/*
Bilinear interpolation in the xy plane:

       v2 ●────────────────● v3       y
          │       *        │          ↑
          │     (x, y)     │          │
       v0 ●────────────────● v1       └──→ x

  row0: y = 0 edge [v0, v1]
  row1: y = 1 edge [v2, v3]
*/
inline float32 bi_mix(
	const float32 v0,
	const float32 v1,
	const float32 v2,
	const float32 v3,
	const float32 x,
	const float32 y)
{
	const float32 row0 = mix(v0, v1, x);
	const float32 row1 = mix(v2, v3, x);
	return mix(row0, row1, y);
}

/*
Trilinear interpolation in the xyz volume:

             v6 ●────────────────● v7
               ╱│               ╱│
              ╱ │              ╱ │
          v2 ●──┼─────────────●v3│
             │  │             │  │
             │  │      *      │  │
             │  ●─────────────┼──●
             │ v4             │  v5
             │ ╱              │ ╱
             │╱               │╱
          v0 ●────────────────● v1

  plane0: z = 0 face [v0, v1, v2, v3]
  plane1: z = 1 face [v4, v5, v6, v7]
  result: * at (x, y, z)
*/
inline float32 tri_mix(
	const float32 v0,
	const float32 v1,
	const float32 v2,
	const float32 v3,
	const float32 v4,
	const float32 v5,
	const float32 v6,
	const float32 v7,
	const float32 x,
	const float32 y,
	const float32 z)
{
	const float32 plane0 = bi_mix(v0, v1, v2, v3, x, y);
	const float32 plane1 = bi_mix(v4, v5, v6, v7, x, y);
	return mix(plane0, plane1, z);
}

/*
Quadrilinear interpolation in xyzw:

  w = 0 hyperplane                         w = 1 hyperplane
  ┌──────────────────────┐                 ┌──────────────────────┐
  │ xyz volume v0 … v7   │                 │ xyz volume v8 … v15  │
  │          * volume0   │                 │          * volume1   │
  └──────────────────────┘                 └──────────────────────┘

  volume0 *──────────────────*──────────────────* volume1
          0                  w                  1
                             ▲ result
*/
inline float32 quad_mix(
	const float32 v0,
	const float32 v1,
	const float32 v2,
	const float32 v3,
	const float32 v4,
	const float32 v5,
	const float32 v6,
	const float32 v7,
	const float32 v8,
	const float32 v9,
	const float32 v10,
	const float32 v11,
	const float32 v12,
	const float32 v13,
	const float32 v14,
	const float32 v15,
	const float32 x,
	const float32 y,
	const float32 z,
	const float32 w)
{
	const float32 volume0 = tri_mix(v0, v1, v2, v3, v4, v5, v6, v7, x, y, z);
	const float32 volume1 = tri_mix(v8, v9, v10, v11, v12, v13, v14, v15, x, y, z);
	return mix(volume0, volume1, w);
}

}// end namespace detail::perlin

inline float32 perlin(const float32 position)
{
	uint32 cellX;
	const float32 fx = detail::perlin::floor_fraction(position, cellX);
	const float32 u = detail::perlin::fade(fx);

	const auto cornerValue =
		[=](const uint32 xOffset)
		{
			return detail::perlin::gradient_dot_1d(
				jenkins_lookup3_32(cellX + xOffset),
				fx - static_cast<float32>(xOffset));
		};

	return detail::perlin::mix(cornerValue(0), cornerValue(1), u);
}

inline float32 perlin(const Vector2F& position)
{
	uint32 cellX;
	uint32 cellY;
	const float32 fx = detail::perlin::floor_fraction(position.x(), cellX);
	const float32 fy = detail::perlin::floor_fraction(position.y(), cellY);
	const float32 u = detail::perlin::fade(fx);
	const float32 v = detail::perlin::fade(fy);

	const auto cornerValue = 
		[=](const uint32 xOffset, const uint32 yOffset)
		{
			const Vector2UI cornerCell(cellX + xOffset, cellY + yOffset);
			return detail::perlin::gradient_dot_2d(
				jenkins_lookup3_32(cornerCell.toView()),
				fx - static_cast<float32>(xOffset),
				fy - static_cast<float32>(yOffset));
		};

	return detail::perlin::bi_mix(
		cornerValue(0, 0), cornerValue(1, 0),
		cornerValue(0, 1), cornerValue(1, 1),
		u, v);
}

inline float32 perlin(const Vector3F& position)
{
	uint32 cellX;
	uint32 cellY;
	uint32 cellZ;
	const float32 fx = detail::perlin::floor_fraction(position.x(), cellX);
	const float32 fy = detail::perlin::floor_fraction(position.y(), cellY);
	const float32 fz = detail::perlin::floor_fraction(position.z(), cellZ);
	const float32 u = detail::perlin::fade(fx);
	const float32 v = detail::perlin::fade(fy);
	const float32 w = detail::perlin::fade(fz);

	const auto cornerValue = 
		[=](const uint32 xOffset, const uint32 yOffset, const uint32 zOffset)
		{
			const Vector3UI cornerCell(cellX + xOffset, cellY + yOffset, cellZ + zOffset);
			return detail::perlin::gradient_dot_3d(
				jenkins_lookup3_32(cornerCell.toView()),
				fx - static_cast<float32>(xOffset),
				fy - static_cast<float32>(yOffset),
				fz - static_cast<float32>(zOffset));
		};

	return detail::perlin::tri_mix(
		cornerValue(0, 0, 0), cornerValue(1, 0, 0),
		cornerValue(0, 1, 0), cornerValue(1, 1, 0),
		cornerValue(0, 0, 1), cornerValue(1, 0, 1),
		cornerValue(0, 1, 1), cornerValue(1, 1, 1),
		u, v, w);
}

inline float32 perlin(const Vector4F& position)
{
	uint32 cellX;
	uint32 cellY;
	uint32 cellZ;
	uint32 cellW;
	const float32 fx = detail::perlin::floor_fraction(position.x(), cellX);
	const float32 fy = detail::perlin::floor_fraction(position.y(), cellY);
	const float32 fz = detail::perlin::floor_fraction(position.z(), cellZ);
	const float32 fw = detail::perlin::floor_fraction(position.w(), cellW);
	const float32 u = detail::perlin::fade(fx);
	const float32 v = detail::perlin::fade(fy);
	const float32 t = detail::perlin::fade(fz);
	const float32 s = detail::perlin::fade(fw);

	const auto cornerValue = 
		[=](const uint32 xOffset, const uint32 yOffset, const uint32 zOffset, const uint32 wOffset)
		{
			const TVector4<uint32> cornerCell(cellX + xOffset, cellY + yOffset, cellZ + zOffset, cellW + wOffset);
			return detail::perlin::gradient_dot_4d(
				jenkins_lookup3_32(cornerCell.toView()),
				fx - static_cast<float32>(xOffset),
				fy - static_cast<float32>(yOffset),
				fz - static_cast<float32>(zOffset),
				fw - static_cast<float32>(wOffset));
		};

	return detail::perlin::quad_mix(
		cornerValue(0, 0, 0, 0), cornerValue(1, 0, 0, 0),
		cornerValue(0, 1, 0, 0), cornerValue(1, 1, 0, 0),
		cornerValue(0, 0, 1, 0), cornerValue(1, 0, 1, 0),
		cornerValue(0, 1, 1, 0), cornerValue(1, 1, 1, 0),
		cornerValue(0, 0, 0, 1), cornerValue(1, 0, 0, 1),
		cornerValue(0, 1, 0, 1), cornerValue(1, 1, 0, 1),
		cornerValue(0, 0, 1, 1), cornerValue(1, 0, 1, 1),
		cornerValue(0, 1, 1, 1), cornerValue(1, 1, 1, 1),
		u, v, t, s);
}

}// end namespace ph::math::noise
