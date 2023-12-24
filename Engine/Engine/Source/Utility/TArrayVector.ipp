#pragma once

#include "Utility/TArrayVector.h"

#include <Common/assertion.h>
#include <Common/config.h>

#include <utility>
#include <type_traits>

namespace ph
{

template<typename T, std::size_t N>
inline TArrayVector<T, N>::TArrayVector() :

#if PH_DEBUG
	// Request value-initialization: set to zeros for primitive types
	m_data{},
#else
	// Intentionally left empty: default-initialize array members
#endif

	m_size(0)
{}

template<typename T, std::size_t N>
template<typename U>
inline void TArrayVector<T, N>::pushBack(U&& item)
{
	// TODO: static assert for U == T, amy require ref and ptr removal

	PH_ASSERT_LT(m_size, m_data.size());

	// FIXME: what if assignment throw? need increment m_size later only if assignment succeeded; 
	// perhaps check if op is no-throw?
	m_data[m_size++] = std::forward<U>(item);
}

template<typename T, std::size_t N>
inline void TArrayVector<T, N>::popBack()
{
	PH_ASSERT_GT(m_size, 0);
	
	--m_size;
}

template<typename T, std::size_t N>
inline std::size_t TArrayVector<T, N>::size() const
{
	PH_ASSERT_LE(m_size, N);

	return m_size;
}

template<typename T, std::size_t N>
inline void TArrayVector<T, N>::clear()
{
	m_size = 0;
}

template<typename T, std::size_t N>
inline bool TArrayVector<T, N>::isEmpty() const
{
	return m_size == 0;
}

template<typename T, std::size_t N>
inline bool TArrayVector<T, N>::isFull() const
{
	return m_size == N;
}

template<typename T, std::size_t N>
inline T* TArrayVector<T, N>::get(const std::size_t index)
{
	return index < m_size ? &(m_data[index]) : nullptr;
}

template<typename T, std::size_t N>
inline const T* TArrayVector<T, N>::get(const std::size_t index) const
{
	return index < m_size ? &(m_data[index]) : nullptr;
}

template<typename T, std::size_t N>
inline T& TArrayVector<T, N>::operator [] (const std::size_t index)
{
	PH_ASSERT_LT(index, m_size);

	return m_data[index];
}

template<typename T, std::size_t N>
inline const T& TArrayVector<T, N>::operator [] (const std::size_t index) const
{
	PH_ASSERT_LT(index, m_size);

	return m_data[index];
}

template<typename T, std::size_t N>
inline T& TArrayVector<T, N>::front()
{
	PH_ASSERT_GT(m_size, 0);

	return (*this)[0];
}

template<typename T, std::size_t N>
inline const T& TArrayVector<T, N>::front() const
{
	PH_ASSERT_GT(m_size, 0);

	return (*this)[0];
}

template<typename T, std::size_t N>
inline T& TArrayVector<T, N>::back()
{
	PH_ASSERT_GT(m_size, 0);

	return (*this)[m_size - 1];
}

template<typename T, std::size_t N>
inline const T& TArrayVector<T, N>::back() const
{
	PH_ASSERT_GT(m_size, 0);

	return (*this)[m_size - 1];
}

template<typename T, std::size_t N>
typename std::array<T, N>::iterator TArrayVector<T, N>::begin() noexcept
{
	return m_data.begin();
}

template<typename T, std::size_t N>
typename std::array<T, N>::const_iterator TArrayVector<T, N>::begin() const noexcept
{
	return m_data.begin();
}

template<typename T, std::size_t N>
typename std::array<T, N>::iterator TArrayVector<T, N>::end() noexcept
{
	// Not using std::advance() as we expect it to be randomly accessible
	// (no permissive code)
	return m_data.begin() + m_size;
}

template<typename T, std::size_t N>
typename std::array<T, N>::const_iterator TArrayVector<T, N>::end() const noexcept
{
	// Not using std::advance() as we expect it to be randomly accessible
	// (no permissive code)
	return m_data.begin() + m_size;
}

}// end namespace ph
