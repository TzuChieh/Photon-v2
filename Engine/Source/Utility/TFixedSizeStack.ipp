#pragma once

#include "Utility/TFixedSizeStack.h"
#include "Common/assertion.h"

namespace ph
{

template<typename T, std::size_t N>
inline TFixedSizeStack<T, N>::TFixedSizeStack() :
	m_data{}, m_currentIndex(-1)
{

}

template<typename T, std::size_t N>
inline TFixedSizeStack<T, N>::TFixedSizeStack(const TFixedSizeStack& other) : 
	m_data(other.m_data), m_currentIndex(other.m_currentIndex)
{

}

template<typename T, std::size_t N>
inline void TFixedSizeStack<T, N>::push(const T& item)
{
	PH_ASSERT(m_currentIndex + 1 >= 0 && m_currentIndex + 1 < static_cast<int32>(N));

	m_data[++m_currentIndex] = item;
}

template<typename T, std::size_t N>
inline void TFixedSizeStack<T, N>::pop()
{
	PH_ASSERT(m_currentIndex - 1 >= -1 && m_currentIndex - 1 < static_cast<int32>(N) - 1);
	
	--m_currentIndex;
}

template<typename T, std::size_t N>
inline T& TFixedSizeStack<T, N>::get()
{
	return m_data[m_currentIndex];
}

template<typename T, std::size_t N>
inline const T& TFixedSizeStack<T, N>::get() const
{
	return m_data[m_currentIndex];
}

template<typename T, std::size_t N>
inline std::size_t TFixedSizeStack<T, N>::height() const
{
	return static_cast<std::size_t>(m_currentIndex + 1);
}

template<typename T, std::size_t N>
inline void TFixedSizeStack<T, N>::clear()
{
	m_currentIndex = -1;
}

template<typename T, std::size_t N>
inline bool TFixedSizeStack<T, N>::isEmpty() const
{
	return m_currentIndex == -1;
}

template<typename T, std::size_t N>
inline TFixedSizeStack<T, N>& TFixedSizeStack<T, N>::operator = (const TFixedSizeStack& rhs)
{
	m_data         = rhs.m_data;
	m_currentIndex = rhs.m_currentIndex;

	return *this;
}

template<typename T, std::size_t N>
inline T& TFixedSizeStack<T, N>::operator [] (const std::size_t index)
{
	PH_ASSERT(index < N);

	return m_data[index];
}

template<typename T, std::size_t N>
inline const T& TFixedSizeStack<T, N>::operator [] (const std::size_t index) const
{
	PH_ASSERT(index < N);

	return m_data[index];
}

}// end namespace ph

