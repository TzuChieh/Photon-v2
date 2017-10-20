#pragma once

#include "Utility/TFixedSizeStack.h"

namespace ph
{

template<typename T, std::size_t N>
inline TFixedSizeStack<T, N>::TFixedSizeStack() :
	m_data{}, m_head(0)
{

}

template<typename T, std::size_t N>
inline TFixedSizeStack<T, N>::TFixedSizeStack(const TFixedSizeStack& other) : 
	m_data(other.m_data), m_head(other.m_head)
{

}

template<typename T, std::size_t N>
inline TFixedSizeStack<T, N>& TFixedSizeStack<T, N>::operator = (const TFixedSizeStack& rhs)
{
	m_data = rhs.m_data;
	m_head = rhs.m_head;

	return *this;
}

}// end namespace ph

