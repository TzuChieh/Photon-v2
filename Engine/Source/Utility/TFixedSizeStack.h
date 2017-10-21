#pragma once

#include "Common/primitive_type.h"

#include <cstddef>
#include <array>

namespace ph
{

template<typename T, std::size_t N>
class TFixedSizeStack final
{
public:
	inline TFixedSizeStack();
	inline TFixedSizeStack(const TFixedSizeStack& other);
	inline ~TFixedSizeStack() = default;

	inline void push(const T& item);
	inline void pop();
	inline T& get();
	inline const T& get() const;
	inline std::size_t height() const;

	inline TFixedSizeStack& operator = (const TFixedSizeStack& rhs);
	inline T& operator [] (std::size_t index);
	inline const T& operator [] (std::size_t index) const;

private:
	std::array<T, N> m_data;
	int32            m_currentIndex;
};

}// end namespace ph

#include "Utility/TFixedSizeStack.ipp"