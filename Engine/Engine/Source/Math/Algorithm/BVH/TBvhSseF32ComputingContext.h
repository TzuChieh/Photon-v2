#pragma once

#include "Math/Algorithm/BVH/TWideBvhNode.h"
#include "Math/Geometry/TAABB3D.h"
#include "Math/TVector3.h"

#include <Common/primitive_type.h>
#include <Common/config.h>
#include <Common/compiler.h>
#include <Common/utility.h>
#include <Common/memory.h>

#include <cstddef>
#include <type_traits>
#include <array>
#include <limits>
#include <concepts>

#if PH_USE_SIMD
#if PH_COMPILER_IS_MSVC
#include <immintrin.h>
#else
#include <x86intrin.h>
#endif
#endif

namespace ph::math
{

/*! @brief A SSE computing context for BVH.
Use `isSupported()` to check the availability of the required hardware feature.
*/
template<std::size_t N, typename Index>
class TBvhSseF32ComputingContext final
{
	static_assert(N >= 2);
	static_assert(std::is_unsigned_v<Index>);

	// Number of batches
	inline static constexpr auto B = N % 4 ? N / 4 + 1 : N / 4;

public:
	/*!
	@return Whether this SSE computing context is supported by the current hardware and configuration.
	Other methods have effect only if this method returns `true`.
	@note This method is always available on every platform.
	*/
	static constexpr bool isSupported()
	{
		return PH_HARDWARE_HAS_SSE4_1 && std::is_same_v<real, float32>;
	}

public:
	[[PH_ALWAYS_INLINE]]
	void setSegment(
		const TVector3<float32>& segmentOrigin,
		const TVector3<float32>& rcpSegmentDir)
	{
#if PH_HARDWARE_HAS_SSE
		for(std::size_t di = 0; di < 3; ++di)
		{
			m_segmentOrigins[di] = _mm_set1_ps(segmentOrigin[di]);
			m_rcpSegmentDirs[di] = _mm_set1_ps(rcpSegmentDir[di]);
		}
#endif
	}

	[[PH_ALWAYS_INLINE]]
	void setNode(const TWideBvhNode<N, Index>& node) requires std::is_same_v<real, float32>
	{
		const auto& emptyAABB = AABB3D::makeEmpty();

#if PH_HARDWARE_HAS_SSE
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

					m_aabbMins[di][ci / 4] = _mm_setr_ps(
						aabb0.getMinVertex()[di],
						aabb1.getMinVertex()[di],
						aabb2.getMinVertex()[di],
						aabb3.getMinVertex()[di]);
					m_aabbMaxs[di][ci / 4] = _mm_setr_ps(
						aabb0.getMaxVertex()[di],
						aabb1.getMaxVertex()[di],
						aabb2.getMaxVertex()[di],
						aabb3.getMaxVertex()[di]);
				}
				else
				{
					m_aabbMins[di][ci / 4] = _mm_load_ps(node.getMinVerticesOnAxis(di).data());
					m_aabbMaxs[di][ci / 4] = _mm_load_ps(node.getMaxVerticesOnAxis(di).data());
				}
			}
		}
#endif
	}

	template<bool IS_ROBUST = true>
	[[PH_ALWAYS_INLINE]]
	void intersectAabbVolumes(const float32 segmentMinT, const float32 segmentMaxT)
	{
		// The implementation is similar to `TAABB3D<T>::intersectVolumeTavian()` and 
		// `TAABB3D<T>::intersectVolumeRobust()`

#if PH_HARDWARE_HAS_SSE
		m_aabbMinTs = make_array<__m128, B>(_mm_set1_ps(segmentMinT));
		m_aabbMaxTs = make_array<__m128, B>(_mm_set1_ps(segmentMaxT));
		for(std::size_t di = 0; di < 3; ++di)
		{
			for(std::size_t bi = 0; bi < B; ++bi)
			{
				const __m128 t1 =
					_mm_mul_ps(_mm_sub_ps(m_aabbMins[di][bi], m_segmentOrigins[di]), m_rcpSegmentDirs[di]);
				const __m128 t2 =
					_mm_mul_ps(_mm_sub_ps(m_aabbMaxs[di][bi], m_segmentOrigins[di]), m_rcpSegmentDirs[di]);

				const __m128 minT = _mm_min_ps(t1, t2);
				const __m128 maxT = _mm_max_ps(t1, t2);

				// Safe max: fallback to `segmentMinT` in case of NaN
				m_aabbMinTs[bi] = _mm_max_ps(minT, m_aabbMinTs[bi]);

				// Safe min: fallback to `segmentMaxT` in case of NaN
				m_aabbMaxTs[bi] = _mm_min_ps(maxT, m_aabbMaxTs[bi]);

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
				m_aabbMaxTs[bi] = _mm_mul_ps(m_aabbMaxTs[bi], _mm_set1_ps(multiplier));
			}
		}
#endif
	}

	/*!
	@return A mask storing the hit result. The i-th bit is 1 if the i-th AABB is hit; 0 otherwise.
	*/
	template<bool IS_ROBUST = true, std::unsigned_integral MaskType = uint32f>
	[[PH_ALWAYS_INLINE]]
	MaskType getIntersectResultAsMask() const
	{
		static_assert(N <= sizeof_in_bits<MaskType>(), "Need more bits for `MaskType`.");

		MaskType hitMask = 0;

#if PH_HARDWARE_HAS_SSE
		for(std::size_t bi = 0; bi < B; ++bi)
		{
			hitMask <<= 4;
			hitMask |= _mm_movemask_ps(_mm_cmple_ps(m_aabbMinTs[bi], m_aabbMaxTs[bi]));
		}
#endif

		return hitMask;
	}

	[[PH_ALWAYS_INLINE]]
	TAlignedArray<float32, B * 4> getIntersectResultAsMinTsOr(const float32 missValue) const
	{
		TAlignedArray<float32, B * 4> results;

#if PH_HARDWARE_HAS_SSE4_1
		for(std::size_t bi = 0; bi < B; ++bi)
		{
			// Perform `value = aabbMinT <= aabbMaxTs ? aabbMaxTs : missValue`
			const __m128 cmpleMask = _mm_cmple_ps(m_aabbMinTs[bi], m_aabbMaxTs[bi]);
			const __m128 values = _mm_blendv_ps(_mm_set1_ps(missValue), m_aabbMinTs[bi], cmpleMask);

			_mm_store_ps(&(results[bi * 4]), values);
		}
#else
		results.fill(missValue);
#endif

		return results;
	}

#if PH_HARDWARE_HAS_SSE
private:
	std::array<std::array<__m128, B>, 3> m_aabbMins;
	std::array<std::array<__m128, B>, 3> m_aabbMaxs;

	std::array<__m128, 3> m_segmentOrigins;
	std::array<__m128, 3> m_rcpSegmentDirs;

	std::array<__m128, B> m_aabbMinTs;
	std::array<__m128, B> m_aabbMaxTs;
#endif
};

}// end namespace ph::math
