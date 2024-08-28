#pragma once

#include "Math/Algorithm/BVH/TWideBvhNode.h"
#include "Math/Geometry/TAABB3D.h"
#include "Math/TVector3.h"

#include <Common/primitive_type.h>
#include <Common/config.h>
#include <Common/compiler.h>
#include <Common/utility.h>

#include <cstddef>
#include <type_traits>
#include <array>
#include <limits>

#if PH_COMPILER_IS_MSVC
#include <immintrin.h>
#else
#include <x86intrin.h>
#endif

namespace ph::math
{

#if PH_CONFIG_HARDWARE_HAS_SSE

template<std::size_t N, typename Index>
class TBvhSseF32Context final
{
	static_assert(N >= 2);
	static_assert(std::is_unsigned_v<Index>);

	// Number of batches
	inline static constexpr auto B = N % 4 ? N / 4 + 1 : N / 4;

public:
	std::array<std::array<__m128, B>, 3> aabbMins;
	std::array<std::array<__m128, B>, 3> aabbMaxs;

	std::array<__m128, 3> segmentOrigins;
	std::array<__m128, 3> rcpSegmentDirs;

	[[PH_ALWAYS_INLINE]]
	void setSegment(
		const TVector3<float>& segmentOrigin,
		const TVector3<float>& rcpSegmentDir)
	{
		for(std::size_t di = 0; di < 3; ++di)
		{
			segmentOrigins[di] = _mm_set1_ps(segmentOrigin[di]);
			rcpSegmentDirs[di] = _mm_set1_ps(rcpSegmentDir[di]);
		}
	}

	[[PH_ALWAYS_INLINE]]
	void setNode(const TWideBvhNode<N, Index>& node) requires std::is_same_v<real, float>
	{
		const auto& emptyAABB = AABB3D::makeEmpty();

		for(std::size_t di = 0; di < 3; ++di)
		{
			for(std::size_t ci = 0; ci < N; ci += 4)
			{
				if constexpr(N % 4)
				{
					const auto& aabb0 = node.getAABB(ci);
					const auto& aabb1 = ci + 1 < N ? node.getAABB(ci + 1) : emptyAABB;
					const auto& aabb2 = ci + 2 < N ? node.getAABB(ci + 2) : emptyAABB;
					const auto& aabb3 = ci + 3 < N ? node.getAABB(ci + 3) : emptyAABB;

					aabbMins[di][ci / 4] = _mm_setr_ps(
						aabb0.getMinVertex()[di],
						aabb1.getMinVertex()[di],
						aabb2.getMinVertex()[di],
						aabb3.getMinVertex()[di]);
					aabbMaxs[di][ci / 4] = _mm_setr_ps(
						aabb0.getMaxVertex()[di],
						aabb1.getMaxVertex()[di],
						aabb2.getMaxVertex()[di],
						aabb3.getMaxVertex()[di]);
				}
				else
				{
					/*aabbMins[di][ci / 4] = _mm_setr_ps(
						node.getAABB(ci).getMinVertex()[di],
						node.getAABB(ci + 1).getMinVertex()[di],
						node.getAABB(ci + 2).getMinVertex()[di],
						node.getAABB(ci + 3).getMinVertex()[di]);
					aabbMaxs[di][ci / 4] = _mm_setr_ps(
						node.getAABB(ci).getMaxVertex()[di],
						node.getAABB(ci + 1).getMaxVertex()[di],
						node.getAABB(ci + 2).getMaxVertex()[di],
						node.getAABB(ci + 3).getMaxVertex()[di]);*/

					aabbMins[di][ci / 4] = _mm_load_ps(node.getMinVerticesOnAxis(di).data());
					aabbMaxs[di][ci / 4] = _mm_load_ps(node.getMaxVerticesOnAxis(di).data());
				}
			}
		}
	}

	/*!
	@return A mask storing the hit result. The i-th bit is 1 if the i-th AABB is hit; 0 otherwise.
	*/
	template<bool IS_ROBUST = true>
	[[PH_ALWAYS_INLINE]]
	uint32f intersectAabbVolumes(const float segmentMinT, const float segmentMaxT) const
	{
		// The implementation is similar to `TAABB3D<T>::intersectVolumeTavian()` and 
		// `TAABB3D<T>::intersectVolumeRobust()`

		auto aabbMinTs = make_array<__m128, B>(_mm_set1_ps(segmentMinT));
		auto aabbMaxTs = make_array<__m128, B>(_mm_set1_ps(segmentMaxT));
		for(std::size_t di = 0; di < 3; ++di)
		{
			for(std::size_t bi = 0; bi < B; ++bi)
			{
				const __m128 t1 =
					_mm_mul_ps(_mm_sub_ps(aabbMins[di][bi], segmentOrigins[di]), rcpSegmentDirs[di]);
				const __m128 t2 =
					_mm_mul_ps(_mm_sub_ps(aabbMaxs[di][bi], segmentOrigins[di]), rcpSegmentDirs[di]);

				const __m128 minT = _mm_min_ps(t1, t2);
				const __m128 maxT = _mm_max_ps(t1, t2);

				// Safe max: fallback to `segmentMinT` in case of NaN
				aabbMinTs[bi] = _mm_max_ps(minT, aabbMinTs[bi]);

				// Safe min: fallback to `segmentMaxT` in case of NaN
				aabbMaxTs[bi] = _mm_min_ps(maxT, aabbMaxTs[bi]);

				// The following links have more information on the behavior of MINPS and MAXPS
				// (they all satisfy the safe requirement)
				// https://www.felixcloutier.com/x86/minps
				// https://tavianator.com/2015/ray_box_nan.html
			}
		}

		if constexpr(IS_ROBUST)
		{
			constexpr auto multiplier = std::numeric_limits<float>::epsilon() * 2 + 1;

			for(std::size_t bi = 0; bi < B; ++bi)
			{
				aabbMaxTs[bi] = _mm_mul_ps(aabbMaxTs[bi], _mm_set1_ps(multiplier));
			}
		}
		
		static_assert(N <= sizeof_in_bits<uint32f>(), "Need more bits for the mask type.");

		uint32f hitMask = 0;
		for(std::size_t bi = 0; bi < B; ++bi)
		{
			hitMask <<= 4;
			hitMask |= _mm_movemask_ps(_mm_cmple_ps(aabbMinTs[bi], aabbMaxTs[bi]));

		}
		return hitMask;
	}
};

#endif

}// end namespace ph::math
