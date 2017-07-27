#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector2.h"

#include <cstddef>

namespace ph
{

class SampleArray
{
public:
	inline SampleArray() :
		m_data(nullptr), m_numElements(0)
	{

	}

	inline SampleArray(real* const data, std::size_t numElements) :
		m_data(data), m_numElements(numElements)
	{

	}

	inline std::size_t numElements() const
	{
		return m_numElements;
	}

	bool isValid() const
	{
		return m_data != nullptr;
	}

protected:
	real*       m_data;
	std::size_t m_numElements;

	inline ~SampleArray() = default;

	void perElementShuffleDurstenfeld(std::size_t dim);
	void perDimensionShuffleDurstenfeld(std::size_t dim);
};

class SampleArray1D final : public SampleArray
{
public:
	using SampleArray::SampleArray;

	void perElementShuffle();
	void perDimensionShuffle();

	inline void set(const std::size_t index, const real value)
	{
		m_data[index] = value;
	}

	inline real operator [] (const std::size_t index) const
	{
		return m_data[index];
	}
};

class SampleArray2D final : public SampleArray
{
public:
	using SampleArray::SampleArray;

	void perElementShuffle();
	void perDimensionShuffle();

	inline void set(const std::size_t index, const real valueX, const real valueY)
	{
		m_data[index * 2]     = valueX;
		m_data[index * 2 + 1] = valueY;
	}

	inline void set(const std::size_t index, const Vector2R& value)
	{
		set(index, value.x, value.y);
	}

	inline Vector2R operator [] (const std::size_t index) const
	{
		return Vector2R(m_data[index * 2], m_data[index * 2 + 1]);
	}
};

}// end namespace ph