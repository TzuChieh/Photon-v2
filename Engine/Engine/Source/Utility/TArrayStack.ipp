#pragma once

#include "Utility/TArrayStack.h"

#include <Common/assertion.h>

#include <utility>

namespace ph
{

template<typename T, std::size_t N>
inline TArrayStack<T, N>::TArrayStack() :
	m_data{}, m_currentIndex(-1)
{}

template<typename T, std::size_t N>
template<typename U>
inline void TArrayStack<T, N>::push(U&& item)
{
	PH_ASSERT_IN_RANGE(m_currentIndex + 1, Index(0), Index(N));

	m_data[++m_currentIndex] = std::forward<U>(item);
}

template<typename T, std::size_t N>
inline void TArrayStack<T, N>::pop()
{
	PH_ASSERT_IN_RANGE(m_currentIndex - 1, Index(-1), Index(N - 1));
	
	--m_currentIndex;
}

template<typename T, std::size_t N>
inline T& TArrayStack<T, N>::top()
{
	PH_ASSERT_IN_RANGE(m_currentIndex, Index(0), Index(N));

	return m_data[m_currentIndex];
}

template<typename T, std::size_t N>
inline const T& TArrayStack<T, N>::top() const
{
	PH_ASSERT_IN_RANGE(m_currentIndex, Index(0), Index(N));

	return m_data[m_currentIndex];
}

template<typename T, std::size_t N>
inline std::size_t TArrayStack<T, N>::height() const
{
	PH_ASSERT_GE(m_currentIndex + 1, Index(0));

	return static_cast<std::size_t>(m_currentIndex + 1);
}

template<typename T, std::size_t N>
inline void TArrayStack<T, N>::clear()
{
	m_currentIndex = -1;
}

template<typename T, std::size_t N>
inline bool TArrayStack<T, N>::isEmpty() const
{
	return m_currentIndex == -1;
}

template<typename T, std::size_t N>
inline T& TArrayStack<T, N>::operator [] (const std::size_t index)
{
	PH_ASSERT_IN_RANGE(index, 0, m_data.size());

	return m_data[index];
}

template<typename T, std::size_t N>
inline const T& TArrayStack<T, N>::operator [] (const std::size_t index) const
{
	PH_ASSERT_IN_RANGE(index, 0, m_data.size());

	return m_data[index];
}

}// end namespace ph
