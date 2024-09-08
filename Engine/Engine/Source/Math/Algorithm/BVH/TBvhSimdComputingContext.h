#pragma once

#include "Math/Algorithm/BVH/TWideBvhNode.h"
#include "Math/Geometry/TAABB3D.h"
#include "Math/TVector3.h"

#include <Common/primitive_type.h>
#include <Common/config.h>
#include <Common/compiler.h>
#include <Common/utility.h>
#include <Common/memory.h>
#include <Common/assertion.h>

#include <cstddef>
#include <type_traits>
#include <array>
#include <limits>
#include <concepts>
#include <utility>

#if PH_USE_SIMD
#if PH_COMPILER_IS_MSVC
#include <immintrin.h>
#else
#include <x86intrin.h>
#endif
#endif

namespace ph::math
{

/*! @brief A SIMD computing context for BVH.
Use `isSupported()` to check the availability of the required hardware feature.
*/
template<std::size_t N, typename Index>
class TBvhSimdComputingContext final
{
	// Developer note: Try not to mix SSE and AVX intrinsics, see https://stackoverflow.com/questions/41303780/why-is-this-sse-code-6-times-slower-without-vzeroupper-on-skylake.

	static_assert(N >= 2);
	static_assert(std::is_unsigned_v<Index>);

#if PH_USE_AVX
	inline static constexpr std::size_t BATCH_SIZE = N <= 4 ? 4 : 8;

	using BFloatType = std::conditional_t<BATCH_SIZE == 4, __m128, __m256>;
#elif PH_USE_SSE
	inline static constexpr std::size_t BATCH_SIZE = 4;

	using BFloatType = __m128;
#else
	inline static constexpr std::size_t BATCH_SIZE = 4;

	using BFloatType = float32;
#endif

	// Number of batches
	inline static constexpr std::size_t B = N % BATCH_SIZE ? N / BATCH_SIZE + 1 : N / BATCH_SIZE;

public:
	/*!
	@return Whether this SIMD computing context is supported by the current hardware and configuration.
	Other methods have effect only if this method returns `true`.
	@note This method is always available on every platform.
	*/
	static constexpr bool isSupported()
	{
		constexpr bool hasSseSupport = PH_USE_SSE4_1 && std::is_same_v<real, float32>;
		constexpr bool hasAvxSupport = PH_USE_AVX && std::is_same_v<real, float32>;
		return hasSseSupport || hasAvxSupport;
	}

public:
	[[PH_ALWAYS_INLINE]]
	void setSegment(
		const TVector3<float32>& segmentOrigin,
		const TVector3<float32>& rcpSegmentDir)
	{
		for(std::size_t di = 0; di < 3; ++di)
		{
			if constexpr(BATCH_SIZE == 4)
			{
#if PH_USE_SSE
				m_segmentOrigins[di] = _mm_set1_ps(segmentOrigin[di]);
				m_rcpSegmentDirs[di] = _mm_set1_ps(rcpSegmentDir[di]);
#endif
			}
			else if constexpr(BATCH_SIZE == 8)
			{
#if PH_USE_AVX
				for(std::size_t di = 0; di < 3; ++di)
				{
					m_segmentOrigins[di] = _mm256_set1_ps(segmentOrigin[di]);
					m_rcpSegmentDirs[di] = _mm256_set1_ps(rcpSegmentDir[di]);
				}
#endif
			}
		}
	}

	[[PH_ALWAYS_INLINE]]
	void setNode(const TWideBvhNode<N, Index>& node) requires std::is_same_v<real, float32>
	{
		const auto& emptyAABB = AABB3D::makeEmpty();

		for(std::size_t di = 0; di < 3; ++di)
		{
			for(std::size_t ci = 0; ci < N; ci += BATCH_SIZE)
			{
				if constexpr(N % BATCH_SIZE)
				{
					const auto& aabb0 = node.getAABB(ci);
					const auto& aabb1 = ci + 1 < N ? node.getAABB(ci + 1) : emptyAABB;
					const auto& aabb2 = ci + 2 < N ? node.getAABB(ci + 2) : emptyAABB;
					const auto& aabb3 = ci + 3 < N ? node.getAABB(ci + 3) : emptyAABB;
					const auto& aabb4 = ci + 4 < N ? node.getAABB(ci + 4) : emptyAABB;
					const auto& aabb5 = ci + 5 < N ? node.getAABB(ci + 5) : emptyAABB;
					const auto& aabb6 = ci + 6 < N ? node.getAABB(ci + 6) : emptyAABB;
					const auto& aabb7 = ci + 7 < N ? node.getAABB(ci + 7) : emptyAABB;

					if constexpr(BATCH_SIZE == 4)
					{
#if PH_USE_SSE
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
#endif
					}
					else if constexpr(BATCH_SIZE == 8)
					{
#if PH_USE_AVX
						m_aabbMins[di][ci / 8] = _mm256_setr_ps(
							aabb0.getMinVertex()[di],
							aabb1.getMinVertex()[di],
							aabb2.getMinVertex()[di],
							aabb3.getMinVertex()[di],
							aabb4.getMinVertex()[di],
							aabb5.getMinVertex()[di],
							aabb6.getMinVertex()[di],
							aabb7.getMinVertex()[di]);
						m_aabbMaxs[di][ci / 8] = _mm256_setr_ps(
							aabb0.getMaxVertex()[di],
							aabb1.getMaxVertex()[di],
							aabb2.getMaxVertex()[di],
							aabb3.getMaxVertex()[di],
							aabb4.getMaxVertex()[di],
							aabb5.getMaxVertex()[di],
							aabb6.getMaxVertex()[di],
							aabb7.getMaxVertex()[di]);
#endif
					}
				}
				else
				{
					if constexpr(BATCH_SIZE == 4)
					{
#if PH_USE_SSE
						PH_ASSERT_GE(node.SOA_VIEW_ALIGNMENT, 16);
						m_aabbMins[di][ci / 4] = _mm_load_ps(&(node.getMinVerticesOnAxis(di)[ci]));
						m_aabbMaxs[di][ci / 4] = _mm_load_ps(&(node.getMaxVerticesOnAxis(di)[ci]));
#endif
					}
					else if constexpr(BATCH_SIZE == 8)
					{
#if PH_USE_AVX
						PH_ASSERT_GE(node.SOA_VIEW_ALIGNMENT, 32);
						m_aabbMins[di][ci / 8] = _mm256_load_ps(&(node.getMinVerticesOnAxis(di)[ci]));
						m_aabbMaxs[di][ci / 8] = _mm256_load_ps(&(node.getMaxVerticesOnAxis(di)[ci]));
#endif
					}
				}
			}
		}
	}

	template<bool IS_ROBUST = true>
	[[PH_ALWAYS_INLINE]]
	void intersectAabbVolumes(const float32 segmentMinT, const float32 segmentMaxT)
	{
		// The implementation is similar to `TAABB3D<T>::intersectVolumeTavian()` and 
		// `TAABB3D<T>::intersectVolumeRobust()`

		if constexpr(BATCH_SIZE == 4)
#if PH_USE_SSE
		{
			m_aabbMinTs = make_array<__m128, B>(_mm_set1_ps(segmentMinT));
			m_aabbMaxTs = make_array<__m128, B>(_mm_set1_ps(segmentMaxT));
#endif
		}
		else if(BATCH_SIZE == 8)
		{
#if PH_USE_AVX
			m_aabbMinTs = make_array<__m256, B>(_mm256_set1_ps(segmentMinT));
			m_aabbMaxTs = make_array<__m256, B>(_mm256_set1_ps(segmentMaxT));
#endif
		}

		for(std::size_t di = 0; di < 3; ++di)
		{
			for(std::size_t bi = 0; bi < B; ++bi)
			{
				if constexpr(BATCH_SIZE == 4)
				{
#if PH_USE_SSE
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
#endif
				}
				else if constexpr(BATCH_SIZE == 8)
				{
#if PH_USE_AVX
					const __m256 t1 =
						_mm256_mul_ps(_mm256_sub_ps(m_aabbMins[di][bi], m_segmentOrigins[di]), m_rcpSegmentDirs[di]);
					const __m256 t2 =
						_mm256_mul_ps(_mm256_sub_ps(m_aabbMaxs[di][bi], m_segmentOrigins[di]), m_rcpSegmentDirs[di]);

					const __m256 minT = _mm256_min_ps(t1, t2);
					const __m256 maxT = _mm256_max_ps(t1, t2);

					// Safe max: fallback to `segmentMinT` in case of NaN
					m_aabbMinTs[bi] = _mm256_max_ps(minT, m_aabbMinTs[bi]);

					// Safe min: fallback to `segmentMaxT` in case of NaN
					m_aabbMaxTs[bi] = _mm256_min_ps(maxT, m_aabbMaxTs[bi]);
#endif
				}

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
				if constexpr(BATCH_SIZE == 4)
				{
#if PH_USE_SSE
					m_aabbMaxTs[bi] = _mm_mul_ps(m_aabbMaxTs[bi], _mm_set1_ps(multiplier));
#endif
				}
				else if constexpr(BATCH_SIZE == 8)
				{
#if PH_USE_AVX
					m_aabbMaxTs[bi] = _mm256_mul_ps(m_aabbMaxTs[bi], _mm256_set1_ps(multiplier));
#endif
				}
			}
		}
	}

	/*!
	@return A mask storing the hit result. The i-th bit is 1 if the i-th AABB is hit; 0 otherwise.
	*/
	template<std::unsigned_integral MaskType = uint32>
	[[PH_ALWAYS_INLINE]]
	auto getIntersectResultAsMask() const
	-> MaskType
	{
		static_assert(N <= sizeof_in_bits<MaskType>(), "Need more bits for `MaskType`.");

		MaskType hitMask = 0;

		for(std::size_t bi = 0; bi < B; ++bi)
		{
			if constexpr(BATCH_SIZE == 4)
			{
#if PH_USE_SSE
				hitMask <<= 4;
				hitMask |= _mm_movemask_ps(_mm_cmple_ps(m_aabbMinTs[bi], m_aabbMaxTs[bi]));
#endif
			}
			else if constexpr(BATCH_SIZE == 8)
			{
#if PH_USE_AVX
				hitMask <<= 8;
				hitMask |= _mm256_movemask_ps(_mm_cmple_ps(m_aabbMinTs[bi], m_aabbMaxTs[bi]));
#endif
			}
		}

		return hitMask;
	}

	[[PH_ALWAYS_INLINE]]
	auto getIntersectResultAsMinTsOr(const float32 missValue) const
	-> TAlignedArray<float32, B * BATCH_SIZE, sizeof(float32) * BATCH_SIZE>
	{
		TAlignedArray<float32, B * BATCH_SIZE, sizeof(float32) * BATCH_SIZE> results;

		// Perform `value = aabbMinT <= aabbMaxTs ? aabbMaxTs : missValue`
		for(std::size_t bi = 0; bi < B; ++bi)
		{
			if constexpr(BATCH_SIZE == 4)
			{
#if PH_USE_SSE4_1
				const __m128 cmpleMask = _mm_cmple_ps(m_aabbMinTs[bi], m_aabbMaxTs[bi]);
				const __m128 values = _mm_blendv_ps(_mm_set1_ps(missValue), m_aabbMinTs[bi], cmpleMask);

				_mm_store_ps(&(results[bi * 4]), values);
#endif
			}
			else if constexpr(BATCH_SIZE == 8)
			{
#if PH_USE_AVX
				const __m256 cmpleMask = _mm256_cmp_ps(m_aabbMinTs[bi], m_aabbMaxTs[bi], _CMP_LE_OQ);
				const __m256 values = _mm256_blendv_ps(_mm256_set1_ps(missValue), m_aabbMinTs[bi], cmpleMask);

				_mm256_store_ps(&(results[bi * 8]), values);
#endif
			}
		}

#if !PH_USE_SSE4_1 && !PH_USE_AVX
		results.fill(missValue);
#endif

		return results;
	}

#if PH_USE_AVX || PH_USE_SSE
private:
	std::array<std::array<BFloatType, B>, 3> m_aabbMins;
	std::array<std::array<BFloatType, B>, 3> m_aabbMaxs;

	std::array<BFloatType, 3> m_segmentOrigins;
	std::array<BFloatType, 3> m_rcpSegmentDirs;

	std::array<BFloatType, B> m_aabbMinTs;
	std::array<BFloatType, B> m_aabbMaxTs;
#endif
};

}// end namespace ph::math
