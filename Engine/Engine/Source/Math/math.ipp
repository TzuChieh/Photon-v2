#pragma once

#include "Math/math.h"

namespace ph::math
{

template<typename T, std::size_t N>
inline T summation(const std::array<T, N>& values, const T initialValue)
{
	return summation(TSpanView<T, N>{values}, initialValue);
}

template<typename T>
inline T summation(const std::vector<T>& values, const T initialValue)
{
	return summation(TSpanView<T>{values}, initialValue);
}

template<typename T, std::size_t EXTENT>
inline T summation(TSpanView<T, EXTENT> values, const T initialValue)
{
	return std::accumulate(values.begin(), values.end(), initialValue);
}

template<typename T, std::size_t N>
inline T product(const std::array<T, N>& values, const T initialValue)
{
	return product(TSpanView<T, N>{values}, initialValue);
}

template<typename T>
inline T product(const std::vector<T>& values, const T initialValue)
{
	return product(TSpanView<T>{values}, initialValue);
}

template<typename T, std::size_t EXTENT>
inline T product(TSpanView<T, EXTENT> values, const T initialValue)
{
	T result = initialValue;
	for(auto&& value : values)
	{
		result *= value;
	}
	return result;
}

template<typename T, std::size_t N>
inline T length(const std::array<T, N>& vec)
{
	return length(TSpanView<T, N>{vec});
}

template<typename T>
inline T length(const std::vector<T>& vec)
{
	return length(TSpanView<T>{vec});
}

template<typename T, std::size_t EXTENT>
inline T length(TSpanView<T, EXTENT> vec)
{
	return std::sqrt(length_squared(vec));
}

template<typename T, std::size_t N>
inline T length_squared(const std::array<T, N>& vec)
{
	return length_squared(TSpanView<T, N>{vec});
}

template<typename T>
inline T length_squared(const std::vector<T>& vec)
{
	return length_squared(TSpanView<T>{vec});
}

template<typename T, std::size_t EXTENT>
inline T length_squared(TSpanView<T, EXTENT> vec)
{
	T result(0);
	for(const T val : vec)
	{
		result += val * val;
	}
	return result;
}

template<typename T, std::size_t N>
inline T p_norm(const std::array<T, N>& vec)
{
	return p_norm(TSpanView<T, N>{vec});
}

template<typename T>
inline T p_norm(const std::vector<T>& vec)
{
	return p_norm(TSpanView<T>{vec});
}

template<std::size_t P, typename T, std::size_t EXTENT>
inline T p_norm(TSpanView<T, EXTENT> vec)
{
	static_assert(P >= 1);

	if constexpr(P == 1)
	{
		T result(0);
		for(const T val : vec)
		{
			result += std::abs(val);
		}
		return result;
	}
	else if constexpr(P == 2)
	{
		return length<T, EXTENT>(vec);
	}
	else
	{
		float64 result(0);
		for(const T val : vec)
		{
			if constexpr(P % 2 == 0)
			{
				result += std::pow(val, P);
			}
			else
			{
				static_assert(P % 2 == 1);

				result += std::pow(std::abs(val), P);
			}
			
		}
		return static_cast<T>(std::pow(result, 1.0 / P));
	}
}

template<typename T, std::size_t N>
inline void normalize(std::array<T, N>& vec)
{
	return normalize(TSpan<T, N>{vec});
}

template<typename T>
inline void normalize(std::vector<T>& vec)
{
	return normalize(TSpan<T>{vec});
}

template<typename T, std::size_t EXTENT>
inline void normalize(TSpan<T, EXTENT> vec)
{
	if constexpr(std::is_floating_point_v<T>)
	{
		const T rcpLen = static_cast<T>(1) / length<T, EXTENT>(vec);
		for(T& val : vec)
		{
			val *= rcpLen;
		}
	}
	else
	{
		static_assert(std::is_integral_v<T>);

		std::fill(vec.begin(), vec.end(), static_cast<T>(0));

		// The only way that normalizing an integer vector will not result in 
		// a 0-vector is that only a single component has finite value, while
		// all other components are zero. We detect these cases and directly
		// returns the result.

		constexpr auto EMPTY_IDX = static_cast<std::size_t>(-1);

		auto nonZeroIdx = EMPTY_IDX;
		for(std::size_t i = 0; i < EXTENT; ++i)
		{
			if(vec[i] != static_cast<T>(0))
			{
				// Found the first non-zero component, record the index
				if(nonZeroIdx == EMPTY_IDX)
				{
					nonZeroIdx = i;
				}
				// This is not the first non-zero component, the result must be 0-vector
				else
				{
					return;
				}
			}
		}

		// Only a single component is != 0
		if(nonZeroIdx != EMPTY_IDX)
		{
			PH_ASSERT_NE(vec[nonZeroIdx], static_cast<T>(0));

			vec[nonZeroIdx] = static_cast<T>(sign(vec[nonZeroIdx]));
		}
	}
}

template<typename T, std::size_t N>
inline T dot_product(const std::array<T, N>& vecA, const std::array<T, N>& vecB)
{
	return dot_product(TSpanView<T, N>{vecA}, TSpanView<T, N>{vecB});
}

template<typename T>
inline T dot_product(const std::vector<T>& vecA, const std::vector<T>& vecB)
{
	return dot_product(TSpanView<T>{vecA}, TSpanView<T>{vecB});
}

template<typename T, std::size_t EXTENT>
inline T dot_product(TSpanView<T, EXTENT> vecA, TSpanView<T, EXTENT> vecB)
{
	PH_ASSERT_EQ(vecA.size(), vecB.size());

	T result(0);
	for(std::size_t i = 0; i < vecA.size(); ++i)
	{
		result += vecA[i] * vecB[i];
	}
	return result;
}

}// end namespace ph::math
