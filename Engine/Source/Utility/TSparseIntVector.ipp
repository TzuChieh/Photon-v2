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

/*! @brief Add an integer value to the vector.
Complexity is $ O \left( N \right) $, where N is the size of the vector. Duplicated values are allowed.
@return Index to the newly-added value.
*/
std::size_t addValue(IntType intValue);

/*! @brief Add a unique integer value to the vector.
Similar to addValue(IntType), except that duplicated values are disallowed.
@return Index to the newly-added value. Empty if the value exists already.
*/
std::optional<std::size_t> addUniqueValue(IntType intValue);

/*! @brief Remove an integer value from the vector.
Complexity is $ O \left( N \right) $, where N is the size of the vector.
@return The index of the just-removed value. If there are more than 1 @p intValue, the returned
index will be the first one. Empty if the value does not exist.
*/
std::optional<std::size_t> removeValue(IntType intValue);

/*! @brief Remove all integer values that are equal to @p intValue from the vector.
Complexity is $ O \left( N \right) $, where N is the size of the vector.
@return The index of the just-removed value. If there are more than 1 @p intValue, the returned
index will be the first one. Empty if the value does not exist.
*/
std::optional<std::size_t> removeValues(IntType intValue);

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

	const std::size_t insertionIndex = binarySearchSmallestInsertionIndex(intValue);
	return get(insertionIndex) == intValue ? insertionIndex : std::nullopt;
}

/*! @brief Check the existence of an integer value.
Behaves similarly to indexOf(IntType). If the index of the integer value is of interest, use of
indexOf(IntType) is recommended for efficiency (avoid finding the integer value twice).
@return `true` if the speficied integer value exists. `false` otherwise.
*/
bool hasValue(IntType intValue) const;

/*! @brief Check how many stored integer values are equal to @p intValue.
@return Number of integer values in the vector that are equal to @p intValue.
*/
std::size_t numValues(IntType intValue) const;

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

}// end namespace ph
