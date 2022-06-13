#pragma once

#include <concepts>
#include <vector>
#include <cstddef>
#include <optional>

namespace ph
{

/*! @brief A container that offers fast value lookup & iteration, while modifications are slow.
The container has continuous memory allocation.
*/
template<std::integral IntType>
class TSparseIntVector final
{
public:
	inline static constexpr std::size_t DEFAULT_INITIAL_CAPACITY = 16;

	TSparseIntVector();
	explicit TSparseIntVector(std::size_t initialCapacity);

	// TODO: bulk add

	/*! @brief Add an integer value to the vector.
	Complexity is $ O \left( N \right) $, where N is the size of the vector. Duplicated values are allowed.
	@return Current index to the newly-added value.
	*/
	std::size_t addValue(IntType intValue);

	/*! @brief Add a unique integer value to the vector.
	Similar to addValue(IntType), except that duplicated `intValue`s are disallowed.
	@return Current index to the newly-added value. Empty if the value exists already.
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

	/*! @brief Get an integer value from the vector using its index.
	Complexity is $ O \left( 1 \right) $.
	@param index Index of the value. Note that the index for each value might change after certain
	operations. Generally it is only guaranteed to be the same if no non-const operation has been
	performed after adding the value.
	@return The integer value on the specified index.
	*/
	IntType get(std::size_t index) const;

	/*! @brief Find the index of an integer value.
	Complexity is $ O \left( logN \right) $, where N is the size of the vector.
	@return The index of the specified integer value. If there are more than 1 @p intValue, the returned
	index will be the first one. Empty if the value does not exist.
	*/
	std::optional<std::size_t> indexOfValue(IntType intValue) const;

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

	/*! @brief Get the number of values in the vector.
	*/
	std::size_t size() const;

	/*! @name Iterators
	Iterators for stored values.
	*/
	///@{
	typename std::vector<IntType>::const_iterator begin() const noexcept;
	typename std::vector<IntType>::const_iterator end() const noexcept;
	///@}

private:
	std::vector<IntType> m_sortedIntValues;

	/*!
	Find the index to the first value x that satisfies `targetValue` <= x.
	*/
	std::size_t binarySearchSmallestInsertionIndex(IntType targetValue) const;

	/*!
	Count how many values are equal to `targetValue` starting from `startingIndex`.
	*/
	std::size_t numIdenticalValuesFrom(std::size_t startingIndex, IntType targetValue) const;
};

}// end namespace ph

#include "Utility/TSparseIntVector.ipp"
