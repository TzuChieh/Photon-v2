#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"

#include <cstddef>

namespace ph
{

class RenderState
{
public:
	using IntegerState = int64;
	using RealState    = float32;

	constexpr static std::size_t NUM_INTEGER_STATES = 4;
	constexpr static std::size_t NUM_REAL_STATES    = 4;

	enum class EType
	{
		INTEGER,
		REAL
	};

	RenderState() = default;

	IntegerState getIntegerState(std::size_t index) const;
	RealState    getRealState(std::size_t index) const;

	void setIntegerState(std::size_t index, IntegerState state);
	void setRealState(std::size_t index, RealState state);

	constexpr static std::size_t numStates(EType type);

private:
	IntegerState m_integerStates[NUM_INTEGER_STATES];
	RealState    m_realStates[NUM_REAL_STATES];
};

// In-header Implementations:

inline auto RenderState::getIntegerState(const std::size_t index) const -> IntegerState
{
	PH_ASSERT_LT(index, NUM_INTEGER_STATES);

	return m_integerStates[index];
}

inline auto RenderState::getRealState(const std::size_t index) const -> RealState
{
	PH_ASSERT_LT(index, NUM_REAL_STATES);

	return m_realStates[index];
}

inline void RenderState::setIntegerState(const std::size_t index, const IntegerState state)
{
	PH_ASSERT_LT(index, NUM_INTEGER_STATES);

	m_integerStates[index] = state;
}

inline void RenderState::setRealState(const std::size_t index, const RealState state)
{
	PH_ASSERT_LT(index, NUM_REAL_STATES);

	m_realStates[index] = state;
}

inline constexpr std::size_t RenderState::numStates(const EType type)
{
	if(type == EType::INTEGER)
	{
		return NUM_INTEGER_STATES;
	}
	else
	{
		return NUM_REAL_STATES;
	}
}

}// end namespace ph