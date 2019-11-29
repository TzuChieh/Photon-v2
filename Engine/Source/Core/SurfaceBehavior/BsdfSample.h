#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Math/TVector2.h"
#include "Math/TArithmeticArray.h"
#include "Core/SampleGenerator/SampleFlow.h"

#include <cstddef>
#include <array>
#include <utility>
#include <type_traits>

namespace ph
{

class BsdfSample final
{
public:
	explicit BsdfSample(SampleFlow& sampleFlow);

	template<typename T = void>
	decltype(auto) get() const;

	real operator [] (std::size_t index) const;

private:
	std::array<real, 2> m_sample;
	SampleFlow&         m_sampleFlow;
};

// In-header Implementations:

inline BsdfSample::BsdfSample(SampleFlow& sampleFlow) :
	m_sample(sampleFlow.flowND<2>()), m_sampleFlow(sampleFlow)
{
	PH_ASSERT_IN_RANGE_INCLUSIVE(m_sample[0], 0.0_r, 1.0_r);
	PH_ASSERT_IN_RANGE_INCLUSIVE(m_sample[1], 0.0_r, 1.0_r);
}

template<typename T>
inline decltype(auto) BsdfSample::get() const
{
	if constexpr(std::is_same_v<T, std::array<real, 2>>)
	{
		return m_sample;
	}
	else if constexpr(std::is_same_v<T, math::Vector2R>)
	{
		return math::Vector2R(m_sample);
	}
	else if constexpr(std::is_same_v<T, math::TArithmeticArray<real, 2>>)
	{
		return math::TArithmeticArray<real, 2>(m_sample);
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

	return m_sample[index];
}

}// end namespace ph
