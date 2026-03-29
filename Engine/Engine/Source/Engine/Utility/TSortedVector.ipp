#pragma once

#include "Engine/Utility/TSortedVector.h"

#include <Common/assertion.h>

#include <algorithm>
#include <utility>

namespace ph
{

template<typename ValueType, typename Index, typename IsLess>
inline TSortedVector<ValueType, Index, IsLess>::TSortedVector(const Index initialCapacity, IsLess isLess)
	: m_sortedValues()
	, m_isLess      (std::move(isLess))
{
	m_sortedValues.reserve(initialCapacity);
}

template<typename ValueType, typename Index, typename IsLess>
inline Index TSortedVector<ValueType, Index, IsLess>::addValue(ValueType value)
{
	PH_ASSERT_LT(m_sortedValues.size(), std::numeric_limits<Index>::max());

	const Index insertionIndex = binarySearchSmallestInsertionIndex(value);
	m_sortedValues.insert(m_sortedValues.begin() + insertionIndex, std::move(value));
	return insertionIndex;
}

template<typename ValueType, typename Index, typename IsLess>
inline std::optional<Index> TSortedVector<ValueType, Index, IsLess>::addUniqueValue(ValueType value)
{
	const Index insertionIndex = binarySearchSmallestInsertionIndex(value);
	
	// `value` already exists
	if(insertionIndex < size() && get(insertionIndex) == value)
	{
		return std::nullopt;
	}
	// remaining cases where `value` does not exist yet, including the case where the vector is empty
	else
	{
		PH_ASSERT_LT(m_sortedValues.size(), std::numeric_limits<Index>::max());

		m_sortedValues.insert(m_sortedValues.begin() + insertionIndex, std::move(value));
		return insertionIndex;
	}
}

template<typename ValueType, typename Index, typename IsLess>
inline std::optional<Index> TSortedVector<ValueType, Index, IsLess>::removeValue(const ValueType& value)
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

template<typename ValueType, typename Index, typename IsLess>
inline std::optional<Index> TSortedVector<ValueType, Index, IsLess>::removeValues(const ValueType& value, Index* const out_numRemovedValues)
{
	const Index firstIndex        = binarySearchSmallestInsertionIndex(value);
	const Index numValuesToRemove = numIdenticalValuesFrom(firstIndex, value);

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

template<typename ValueType, typename Index, typename IsLess>
inline ValueType TSortedVector<ValueType, Index, IsLess>::removeValueByIndex(const Index index)
{
	PH_ASSERT_LT(index, size());

	ValueType value = std::move(m_sortedValues[index]);
	m_sortedValues.erase(m_sortedValues.begin() + index);
	return value;
}

template<typename ValueType, typename Index, typename IsLess>
inline const ValueType& TSortedVector<ValueType, Index, IsLess>::get(const Index index) const
{
	PH_ASSERT_LT(index, size());
	return m_sortedValues[index];
}

template<typename ValueType, typename Index, typename IsLess>
inline std::optional<Index> TSortedVector<ValueType, Index, IsLess>::indexOfValue(const ValueType& value) const
{
	const Index firstIndex = binarySearchSmallestInsertionIndex(value);
	if(firstIndex < size())
	{
		return get(firstIndex) == value ? std::make_optional(firstIndex) : std::nullopt;
	}
	// reached when vector is empty or `value` is larger than all contained values
	else
	{
		return std::nullopt;
	}
}

template<typename ValueType, typename Index, typename IsLess>
inline bool TSortedVector<ValueType, Index, IsLess>::hasValue(const ValueType& value) const
{
	return indexOfValue(value).has_value();
}

template<typename ValueType, typename Index, typename IsLess>
inline Index TSortedVector<ValueType, Index, IsLess>::numValues(const ValueType& value) const
{
	const Index firstIndex = binarySearchSmallestInsertionIndex(value);
	return numIdenticalValuesFrom(firstIndex, value);
}

template<typename ValueType, typename Index, typename IsLess>
inline Index TSortedVector<ValueType, Index, IsLess>::lowerBound(const ValueType& inputValue) const
{
	return binarySearchSmallestInsertionIndex(inputValue);
}

template<typename ValueType, typename Index, typename IsLess>
inline Index TSortedVector<ValueType, Index, IsLess>::size() const
{
	return static_cast<Index>(m_sortedValues.size());
}

template<typename ValueType, typename Index, typename IsLess>
inline Index TSortedVector<ValueType, Index, IsLess>::capacity() const
{
	return static_cast<Index>(m_sortedValues.capacity());
}

template<typename ValueType, typename Index, typename IsLess>
inline bool TSortedVector<ValueType, Index, IsLess>::isEmpty() const
{
	return m_sortedValues.empty();
}

template<typename ValueType, typename Index, typename IsLess>
inline void TSortedVector<ValueType, Index, IsLess>::shrinkToFit()
{
	m_sortedValues.shrink_to_fit();
}

template<typename ValueType, typename Index, typename IsLess>
inline Index TSortedVector<ValueType, Index, IsLess>::binarySearchSmallestInsertionIndex(const ValueType& targetValue) const
{
	// Search for first value x such that x >= `targetValue`
	const auto lowerBound = std::lower_bound(
		m_sortedValues.begin(), m_sortedValues.end(), targetValue,
		[this](const ValueType& valueA, const ValueType& valueB)
		{
			return m_isLess(valueA, valueB);
		});

	return static_cast<Index>(lowerBound - m_sortedValues.begin());
}

template<typename ValueType, typename Index, typename IsLess>
inline Index TSortedVector<ValueType, Index, IsLess>::numIdenticalValuesFrom(const Index startingIndex, const ValueType& targetValue) const
{
	Index valueCount = 0;
	for(Index i = startingIndex; i < size(); ++i)
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

template<typename ValueType, typename Index, typename IsLess>
inline typename std::vector<ValueType>::const_iterator TSortedVector<ValueType, Index, IsLess>::begin() const noexcept
{
	return m_sortedValues.begin();
}

template<typename ValueType, typename Index, typename IsLess>
inline typename std::vector<ValueType>::const_iterator TSortedVector<ValueType, Index, IsLess>::end() const noexcept
{
	return m_sortedValues.end();
}

}// end namespace ph
