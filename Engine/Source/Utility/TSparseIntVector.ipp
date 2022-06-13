#pragma once

#include "Utility/TSparseIntVector.h"
#include "Common/assertion.h"

#include <algorithm>

namespace ph
{

template<std::integral IntType>
inline TSparseIntVector<IntType>::TSparseIntVector()
	: TSparseIntVector(DEFAULT_INITIAL_CAPACITY)
{}

template<std::integral IntType>
inline TSparseIntVector<IntType>::TSparseIntVector(const std::size_t initialCapacity)
	: m_intValues()
{
	m_sortedIntValues.reserve(initialCapacity);
}

template<std::integral IntType>
inline std::size_t TSparseIntVector<IntType>::addValue(const IntType intValue)
{
	const std::size_t insertionIndex = binarySearchSmallestInsertionIndex(intValue);
	m_sortedIntValues.insert(m_sortedIntValues.begin() + insertionIndex, intValue);
	return insertionIndex;
}

template<std::integral IntType>
inline std::optional<std::size_t> TSparseIntVector<IntType>::addUniqueValue(const IntType intValue)
{
	if(m_sortedIntValues.empty())
	{
		return addValue(intValue);
	}

	const std::size_t insertionIndex = binarySearchSmallestInsertionIndex(intValue);
	if(get(insertionIndex) == intValue)
	{
		return std::nullopt;
	}
	else
	{
		m_sortedIntValues.insert(m_sortedIntValues.begin() + insertionIndex, intValue);
		return insertionIndex;
	}
}

template<std::integral IntType>
inline std::optional<std::size_t> TSparseIntVector<IntType>::removeValue(const IntType intValue)
{
	const auto optFirstValueIndex = indexOfValue(intValue);
	if(!optFirstValueIndex)
	{
		return std::nullopt;
	}

	PH_ASSERT(optFirstValueIndex.has_value());
	m_sortedIntValues.erase(m_sortedIntValues.begin() + *optFirstValueIndex);
	return *optFirstValueIndex;
}

template<std::integral IntType>
inline std::optional<std::size_t> TSparseIntVector<IntType>::removeValues(const IntType intValue)
{
	const std::size_t firstIndex        = binarySearchSmallestInsertionIndex(intValue);
	const std::size_t numValuesToRemove = numIdenticalValuesFrom(firstIndex, intValue);

	// Handles the cases where `m_sortedIntValues` is empty and/or `intValue` is not found
	m_sortedIntValues.erase(
		m_sortedIntValues.begin() + firstIndex,
		m_sortedIntValues.begin() + firstIndex + numValuesToRemove);

	return numValuesToRemove > 0 ? firstIndex : std::nullopt;
}

template<std::integral IntType>
inline IntType TSparseIntVector<IntType>::get(const std::size_t index) const
{
	PH_ASSERT_LT(index, m_sortedIntValues.size());
	return m_sortedIntValues[index];
}

template<std::integral IntType>
inline std::optional<std::size_t> TSparseIntVector<IntType>::indexOfValue(const IntType intValue) const
{
	if(m_sortedIntValues.empty())
	{
		return std::nullopt;
	}

	const std::size_t firstIndex = binarySearchSmallestInsertionIndex(intValue);
	return get(firstIndex) == intValue ? firstIndex : std::nullopt;
}

template<std::integral IntType>
inline bool TSparseIntVector<IntType>::hasValue(const IntType intValue) const
{
	return indexOfValue(intValue).has_value();
}

template<std::integral IntType>
inline std::size_t TSparseIntVector<IntType>::numValues(const IntType intValue) const
{
	const std::size_t firstIndex = binarySearchSmallestInsertionIndex(intValue);
	return numIdenticalValuesFrom(firstIndex, intValue);
}

template<std::integral IntType>
inline std::size_t TSparseIntVector<IntType>::size() const
{
	return m_sortedIntValues.size();
}

template<std::integral IntType>
inline std::size_t TSparseIntVector<IntType>::binarySearchSmallestInsertionIndex(const IntType targetValue) const
{
	// Search for first value x such that `targetValue` <= x
	const auto lowerBound = std::lower_bound(m_sortedIntValues.begin(), m_sortedIntValues.end(), targetValue);

	return static_cast<std::size_t>(lowerBound - m_sortedIntValues.begin());
}

template<std::integral IntType>
inline std::size_t TSparseIntVector<IntType>::numIdenticalValuesFrom(const std::size_t startingIndex, const IntType targetValue) const
{
	std::size_t valueCount = 0;
	for(std::size_t i = startingIndex; i < m_sortedIntValues.size(); ++i)
	{
		if(get(i) == targetValue)
		{
			++valueCount;
		}
		else
		{
			break;
		}
	}
	return valueCount;
}

template<std::integral IntType>
inline typename std::vector<IntType>::const_iterator TSparseIntVector<IntType>::begin() const noexcept
{
	return m_sortedIntValues.begin();
}

template<std::integral IntType>
inline typename std::vector<IntType>::const_iterator TSparseIntVector<IntType>::end() const noexcept
{
	return m_sortedIntValues.end();
}

}// end namespace ph
