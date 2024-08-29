#pragma once

#include "Utility/TArrayHeap.h"

#include <Common/assertion.h>

#include <utility>
#include <algorithm>

namespace ph
{

template<typename T, std::size_t N, typename IsLess>
inline TArrayHeap<T, N, IsLess>::TArrayHeap() requires std::default_initializable<IsLess>
	: TArrayHeap(IsLess{})
{}

template<typename T, std::size_t N, typename IsLess>
inline TArrayHeap<T, N, IsLess>::TArrayHeap(IsLess isLess)
	: m_data{}
	, m_currentIndex(-1)
	, m_isLess(std::move(isLess))
{}

template<typename T, std::size_t N, typename IsLess>
template<typename U>
inline void TArrayHeap<T, N, IsLess>::push(U&& item)
{
	PH_ASSERT_IN_RANGE(m_currentIndex + 1, Index(0), Index(N));

	m_data[++m_currentIndex] = std::forward<U>(item);
	std::push_heap(m_data.begin(), m_data.begin() + m_currentIndex + 1, m_isLess);
}

template<typename T, std::size_t N, typename IsLess>
inline void TArrayHeap<T, N, IsLess>::pop()
{
	PH_ASSERT_IN_RANGE(m_currentIndex - 1, Index(-1), Index(N - 1));
	
	std::pop_heap(m_data.begin(), m_data.begin() + m_currentIndex + 1, m_isLess);
	--m_currentIndex;
}

template<typename T, std::size_t N, typename IsLess>
inline T& TArrayHeap<T, N, IsLess>::top()
{
	PH_ASSERT_IN_RANGE(m_currentIndex, Index(0), Index(N));

	return m_data[0];
}

template<typename T, std::size_t N, typename IsLess>
inline const T& TArrayHeap<T, N, IsLess>::top() const
{
	PH_ASSERT_IN_RANGE(m_currentIndex, Index(0), Index(N));

	return m_data[0];
}

template<typename T, std::size_t N, typename IsLess>
inline std::size_t TArrayHeap<T, N, IsLess>::size() const
{
	PH_ASSERT_GE(m_currentIndex + 1, Index(0));

	return static_cast<std::size_t>(m_currentIndex + 1);
}

template<typename T, std::size_t N, typename IsLess>
inline void TArrayHeap<T, N, IsLess>::clear()
{
	m_currentIndex = -1;
}

template<typename T, std::size_t N, typename IsLess>
inline bool TArrayHeap<T, N, IsLess>::isEmpty() const
{
	return m_currentIndex == -1;
}

}// end namespace ph
