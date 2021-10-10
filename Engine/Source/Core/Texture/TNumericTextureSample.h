#pragma once

#include "Common/config.h"

#include <array>
#include <cstddef>
#include <utility>
#include <stdexcept>
#include <format>

namespace ph
{

template<typename T>
class TNumericTextureSample final
{
public:
	inline static constexpr std::size_t MAX_ELEMENTS = PH_NUMERIC_TEXTURE_MAX_ELEMENTS;

	TNumericTextureSample();

	template<std::size_t N>
	TNumericTextureSample(std::array<T, N> values);

	template<std::size_t N>
	std::array<T, N> getValues() const;

	std::size_t numValues() const;

private:
	std::array<T, MAX_ELEMENTS> m_valueBuffer;
	std::size_t                 m_numValues;
};

// In-header Implementations:

template<typename T>
inline TNumericTextureSample<T>::TNumericTextureSample() :
	m_valueBuffer(),
	m_numValues  (0)
{}

template<typename T>
template<std::size_t N>
inline TNumericTextureSample<T>::TNumericTextureSample(std::array<T, N> values) :
	TNumericTextureSample()
{
	static_assert(N <= MAX_ELEMENTS,
		"Number of input values overflow the internal buffer.");

	for(std::size_t i = 0; i < N; ++i)
	{
		m_valueBuffer[i] = std::move(values[i]);
	}

	m_numValues = N;
}

template<typename T>
template<std::size_t N>
inline auto TNumericTextureSample<T>::getValues() const
-> std::array<T, N>
{
	if(N > m_numValues)
	{
		throw std::out_of_range(std::format(
			"Attepmting to get {} input values from an internal buffer of size {}", N, m_numValues));
	}

	std::array<T, N> values;
	for(std::size_t i = 0; i < N; ++i)
	{
		values[i] = m_valueBuffer[i];
	}
	return values;
}

template<typename T>
inline std::size_t TNumericTextureSample<T>::numValues() const
{
	return m_numValues;
}

}// end namespace ph
