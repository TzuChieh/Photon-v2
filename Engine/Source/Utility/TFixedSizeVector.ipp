#pragma once

#include "Utility/TFixedSizeVector.h"
#include "Common/config.h"
#include "Common/assertion.h"

#include <utility>

namespace ph
{

template<typename T, std::size_t N>
inline TFixedSizeVector<T, N>::TFixedSizeVector() :

#ifdef PH_DEBUG
	// Request value-initialization: set to zeros for primitive types
	m_data{},
#else
	// Intentionally left empty: default-initialize array members
#endif

	m_size(0)
{}

template<typename T, std::size_t N>
inline TFixedSizeVector<T, N>::TFixedSizeVector(const TFixedSizeVector& other) :
	m_data(other.m_data), m_size(other.m_size)
{}

template<typename T, std::size_t N>
inline void TFixedSizeVector<T, N>::pushBack(T&& item)
{
	PH_ASSERT_LT(m_size, m_data.size());

	// FIXME: what if assignment throw? need increment m_size later only if assignment succeeded; 
	// perhaps check if op is no-throw?
	m_data[++m_size] = std::forward<T>(item);
}

template<typename T, std::size_t N>
inline void TFixedSizeVector<T, N>::popBack()
{
	PH_ASSERT_GT(m_size, 0);
	
	--m_size;
}

template<typename T, std::size_t N>
inline std::size_t TFixedSizeVector<T, N>::size() const
{
	return m_size;
}

template<typename T, std::size_t N>
inline void TFixedSizeVector<T, N>::clear()
{
	m_size = 0;
}

template<typename T, std::size_t N>
inline bool TFixedSizeVector<T, N>::isEmpty() const
{
	return m_size == 0;
}

template<typename T, std::size_t N>
inline TFixedSizeVector<T, N>& TFixedSizeVector<T, N>::operator = (const TFixedSizeVector& rhs)
{
	m_data = rhs.m_data;
	m_size = rhs.m_size;

	return *this;
}

template<typename T, std::size_t N>
inline T& TFixedSizeVector<T, N>::operator [] (const std::size_t index)
{
	PH_ASSERT_LT(index, m_size);

	return m_data[index];
}

template<typename T, std::size_t N>
inline const T& TFixedSizeVector<T, N>::operator [] (const std::size_t index) const
{
	PH_ASSERT_LT(index, m_size);

	return m_data[index];
}

template<typename T, std::size_t N>
inline T& TFixedSizeVector<T, N>::front()
{
	PH_ASSERT_GT(m_size, 0);

	return (*this)[0];
}

template<typename T, std::size_t N>
inline const T& TFixedSizeVector<T, N>::front() const
{
	PH_ASSERT_GT(m_size, 0);

	return (*this)[0];
}

template<typename T, std::size_t N>
inline T& TFixedSizeVector<T, N>::back()
{
	PH_ASSERT_GT(m_size, 0);

	return (*this)[m_size - 1];
}

template<typename T, std::size_t N>
inline const T& TFixedSizeVector<T, N>::back() const
{
	PH_ASSERT_GT(m_size, 0);

	return (*this)[m_size - 1];
}

}// end namespace ph
