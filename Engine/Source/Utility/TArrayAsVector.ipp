#pragma once

#include "Utility/TArrayAsVector.h"
#include "Common/config.h"
#include "Common/assertion.h"

#include <utility>

namespace ph
{

template<typename T, std::size_t N>
inline TArrayAsVector<T, N>::TArrayAsVector() :

#ifdef PH_DEBUG
	// Request value-initialization: set to zeros for primitive types
	m_data{},
#else
	// Intentionally left empty: default-initialize array members
#endif

	m_size(0)
{}

template<typename T, std::size_t N>
inline void TArrayAsVector<T, N>::pushBack(T&& item)
{
	PH_ASSERT_LT(m_size, m_data.size());

	// FIXME: what if assignment throw? need increment m_size later only if assignment succeeded; 
	// perhaps check if op is no-throw?
	m_data[++m_size] = std::forward<T>(item);
}

template<typename T, std::size_t N>
inline void TArrayAsVector<T, N>::popBack()
{
	PH_ASSERT_GT(m_size, 0);
	
	--m_size;
}

template<typename T, std::size_t N>
inline std::size_t TArrayAsVector<T, N>::size() const
{
	PH_ASSERT_LE(m_size, N);

	return m_size;
}

template<typename T, std::size_t N>
inline void TArrayAsVector<T, N>::clear()
{
	m_size = 0;
}

template<typename T, std::size_t N>
inline bool TArrayAsVector<T, N>::isEmpty() const
{
	return m_size == 0;
}

template<typename T, std::size_t N>
inline bool TArrayAsVector<T, N>::isFull() const
{
	return m_size == N;
}

template<typename T, std::size_t N>
inline T* TArrayAsVector<T, N>::get(const std::size_t index)
{
	return index < m_size ? &(m_data[index]) : nullptr;
}

template<typename T, std::size_t N>
inline const T* TArrayAsVector<T, N>::get(const std::size_t index) const
{
	return index < m_size ? &(m_data[index]) : nullptr;
}

template<typename T, std::size_t N>
inline T& TArrayAsVector<T, N>::operator [] (const std::size_t index)
{
	PH_ASSERT_LT(index, m_size);

	return m_data[index];
}

template<typename T, std::size_t N>
inline const T& TArrayAsVector<T, N>::operator [] (const std::size_t index) const
{
	PH_ASSERT_LT(index, m_size);

	return m_data[index];
}

template<typename T, std::size_t N>
inline T& TArrayAsVector<T, N>::front()
{
	PH_ASSERT_GT(m_size, 0);

	return (*this)[0];
}

template<typename T, std::size_t N>
inline const T& TArrayAsVector<T, N>::front() const
{
	PH_ASSERT_GT(m_size, 0);

	return (*this)[0];
}

template<typename T, std::size_t N>
inline T& TArrayAsVector<T, N>::back()
{
	PH_ASSERT_GT(m_size, 0);

	return (*this)[m_size - 1];
}

template<typename T, std::size_t N>
inline const T& TArrayAsVector<T, N>::back() const
{
	PH_ASSERT_GT(m_size, 0);

	return (*this)[m_size - 1];
}

}// end namespace ph
