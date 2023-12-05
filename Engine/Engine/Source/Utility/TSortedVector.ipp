#pragma once

#include "Utility/TSortedVector.h"

#include <Common/assertion.h>

#include <algorithm>
#include <utility>

namespace ph
{

template<typename ValueType, typename IsLess>
inline TSortedVector<ValueType, IsLess>::TSortedVector(const std::size_t initialCapacity, IsLess isLess)
	: m_sortedValues()
	, m_isLess      (std::move(isLess))
{
	m_sortedValues.reserve(initialCapacity);
}

template<typename ValueType, typename IsLess>
inline std::size_t TSortedVector<ValueType, IsLess>::addValue(ValueType value)
{
	const std::size_t insertionIndex = binarySearchSmallestInsertionIndex(value);
	m_sortedValues.insert(m_sortedValues.begin() + insertionIndex, std::move(value));
	return insertionIndex;
}

template<typename ValueType, typename IsLess>
inline std::optional<std::size_t> TSortedVector<ValueType, IsLess>::addUniqueValue(ValueType value)
{
	const std::size_t insertionIndex = binarySearchSmallestInsertionIndex(value);
	
	// `value` already exists
	if(insertionIndex < m_sortedValues.size() && get(insertionIndex) == value)
	{
		return std::nullopt;
	}
	// remaining cases where `value` does not exist yet, including the case where the vector is empty
	else
	{
		m_sortedValues.insert(m_sortedValues.begin() + insertionIndex, std::move(value));
		return insertionIndex;
	}
}

template<typename ValueType, typename IsLess>
inline std::optional<std::size_t> TSortedVector<ValueType, IsLess>::removeValue(const ValueType& value)
{
	const auto optFirstValueIndex = indexOfValue(value);
	if(!optFirstValueIndex.has_value())
	{
		return std::nullopt;
	}

	PH_ASSERT(optFirstValueIndex.has_value());
	m_sortedValues.erase(m_sortedValues.begin() + *optFirstValueIndex);
	return *optFirstValueIndex;
}

template<typename ValueType, typename IsLess>
inline std::optional<std::size_t> TSortedVector<ValueType, IsLess>::removeValues(const ValueType& value, std::size_t* const out_numRemovedValues)
{
	const std::size_t firstIndex        = binarySearchSmallestInsertionIndex(value);
	const std::size_t numValuesToRemove = numIdenticalValuesFrom(firstIndex, value);

	// Handles the cases where `m_sortedValues` is empty and/or `value` is not found
	m_sortedValues.erase(
		m_sortedValues.begin() + firstIndex,
		m_sortedValues.begin() + firstIndex + numValuesToRemove);

	if(out_numRemovedValues)
	{
		*out_numRemovedValues = numValuesToRemove;
	}

	return numValuesToRemove > 0 ? std::make_optional(firstIndex) : std::nullopt;
}

template<typename ValueType, typename IsLess>
inline ValueType TSortedVector<ValueType, IsLess>::removeValueByIndex(const std::size_t index)
{
	PH_ASSERT_LT(index, m_sortedValues.size());

	ValueType value = std::move(m_sortedValues[index]);
	m_sortedValues.erase(m_sortedValues.begin() + index);
	return value;
}

template<typename ValueType, typename IsLess>
inline const ValueType& TSortedVector<ValueType, IsLess>::get(const std::size_t index) const
{
	PH_ASSERT_LT(index, m_sortedValues.size());
	return m_sortedValues[index];
}

template<typename ValueType, typename IsLess>
inline std::optional<std::size_t> TSortedVector<ValueType, IsLess>::indexOfValue(const ValueType& value) const
{
	const std::size_t firstIndex = binarySearchSmallestInsertionIndex(value);
	if(firstIndex < m_sortedValues.size())
	{
		return get(firstIndex) == value ? std::make_optional(firstIndex) : std::nullopt;
	}
	// reached when vector is empty or `value` is larger than all contained values
	else
	{
		return std::nullopt;
	}
}

template<typename ValueType, typename IsLess>
inline bool TSortedVector<ValueType, IsLess>::hasValue(const ValueType& value) const
{
	return indexOfValue(value).has_value();
}

template<typename ValueType, typename IsLess>
inline std::size_t TSortedVector<ValueType, IsLess>::numValues(const ValueType& value) const
{
	const std::size_t firstIndex = binarySearchSmallestInsertionIndex(value);
	return numIdenticalValuesFrom(firstIndex, value);
}

template<typename ValueType, typename IsLess>
inline std::size_t TSortedVector<ValueType, IsLess>::size() const
{
	return m_sortedValues.size();
}

template<typename ValueType, typename IsLess>
inline bool TSortedVector<ValueType, IsLess>::isEmpty() const
{
	return m_sortedValues.empty();
}

template<typename ValueType, typename IsLess>
inline std::size_t TSortedVector<ValueType, IsLess>::binarySearchSmallestInsertionIndex(const ValueType& targetValue) const
{
	// Search for first value x such that x >= `targetValue`
	const auto lowerBound = std::lower_bound(
		m_sortedValues.begin(), m_sortedValues.end(), targetValue,
		[this](const ValueType& valueA, const ValueType& valueB)
		{
			return m_isLess(valueA, valueB);
		});

	return static_cast<std::size_t>(lowerBound - m_sortedValues.begin());
}

template<typename ValueType, typename IsLess>
inline std::size_t TSortedVector<ValueType, IsLess>::numIdenticalValuesFrom(const std::size_t startingIndex, const ValueType& targetValue) const
{
	std::size_t valueCount = 0;
	for(std::size_t i = startingIndex; i < m_sortedValues.size(); ++i)
	{
		if(m_sortedValues[i] == targetValue)
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

template<typename ValueType, typename IsLess>
inline typename std::vector<ValueType>::const_iterator TSortedVector<ValueType, IsLess>::begin() const noexcept
{
	return m_sortedValues.begin();
}

template<typename ValueType, typename IsLess>
inline typename std::vector<ValueType>::const_iterator TSortedVector<ValueType, IsLess>::end() const noexcept
{
	return m_sortedValues.end();
}

}// end namespace ph
