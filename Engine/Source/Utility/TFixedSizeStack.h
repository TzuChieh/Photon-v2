#pragma once

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

	inline void push();
	inline void pop();
	inline T& getTop();
	inline const T& getTop() const;

	inline TFixedSizeStack& operator = (const TFixedSizeStack& rhs);

private:
	std::array<T, N> m_data;
	std::size_t      m_head;
};

}// end namespace ph