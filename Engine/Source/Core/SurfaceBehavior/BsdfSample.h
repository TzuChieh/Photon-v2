#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Math/TVector2.h"
#include "Math/TArithmeticArray.h"

#include <cstddef>
#include <array>
#include <utility>
#include <type_traits>

namespace ph
{

class BsdfSample final
{
public:
	explicit BsdfSample(std::array<real, 2> samples);

	template<typename T = void>
	decltype(auto) get() const;

	real operator [] (std::size_t index) const;

private:
	std::array<real, 2> m_samples;
};

// In-header Implementations:

inline BsdfSample::BsdfSample(std::array<real, 2> samples) : 
	m_samples(std::move(samples))
{
	PH_ASSERT_IN_RANGE_INCLUSIVE(m_samples[0], 0.0_r, 1.0_r);
	PH_ASSERT_IN_RANGE_INCLUSIVE(m_samples[1], 0.0_r, 1.0_r);
}

template<typename T>
inline decltype(auto) BsdfSample::get() const
{
	if constexpr(std::is_same_v<T, std::array<real, 2>>)
	{
		return m_samples;
	}
	else if constexpr(std::is_same_v<T, math::Vector2R>)
	{
		return math::Vector2R(m_samples);
	}
	else if constexpr(std::is_same_v<T, math::TArithmeticArray<real, 2>>)
	{
		return math::TArithmeticArray<real, 2>(m_samples);
	}
	// T is unsupported, no conversion can be made
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
		return nullptr;
	}
}

inline real BsdfSample::operator [] (const std::size_t index) const
{
	PH_ASSERT_IN_RANGE(index, 0, 2);

	return m_samples[index];
}

}// end namespace ph
