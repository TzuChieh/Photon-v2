#pragma once

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cstddef>

namespace ph
{

class RenderStats final
{
public:
	using IntegerType = int64;
	using RealType = float32;

	constexpr static std::size_t NUM_INTEGERS = 4;
	constexpr static std::size_t NUM_REALS = 4;

	enum class EType
	{
		INTEGER,
		REAL
	};

	RenderStats() = default;

	IntegerType getInteger(std::size_t index) const;
	RealType getReal(std::size_t index) const;

	void setInteger(std::size_t index, IntegerType value);
	void setReal(std::size_t index, RealType value);

	constexpr static std::size_t numStats(EType type);

private:
	IntegerType m_integers[NUM_INTEGERS];
	RealType m_reals[NUM_REALS];
};

// In-header Implementations:

inline auto RenderStats::getInteger(const std::size_t index) const -> IntegerType
{
	PH_ASSERT_LT(index, NUM_INTEGERS);

	return m_integers[index];
}

inline auto RenderStats::getReal(const std::size_t index) const -> RealType
{
	PH_ASSERT_LT(index, NUM_REALS);

	return m_reals[index];
}

inline void RenderStats::setInteger(const std::size_t index, const IntegerType value)
{
	PH_ASSERT_LT(index, NUM_INTEGERS);

	m_integers[index] = value;
}

inline void RenderStats::setReal(const std::size_t index, const RealType value)
{
	PH_ASSERT_LT(index, NUM_REALS);

	m_reals[index] = value;
}

inline constexpr std::size_t RenderStats::numStats(const EType type)
{
	if(type == EType::INTEGER)
	{
		return NUM_INTEGERS;
	}
	else
	{
		return NUM_REALS;
	}
}

}// end namespace ph
