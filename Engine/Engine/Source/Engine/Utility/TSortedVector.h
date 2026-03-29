#pragma once

#include <Common/compiler.h>

#include <vector>
#include <cstddef>
#include <optional>
#include <functional>
#include <concepts>

namespace ph
{

/*! @brief A sorted container that offers fast value lookup & iteration, while modifications are slow.
The container has continuous memory allocation. The values are stored in ascending order.
*/
template<typename ValueType, typename Index = std::size_t, typename IsLess = std::less<ValueType>>
class TSortedVector final
{
public:
	inline TSortedVector() requires std::default_initializable<IsLess>
		: TSortedVector(0, IsLess{})
	{}

	inline explicit TSortedVector(const Index initialCapacity) requires std::default_initializable<IsLess>
		: TSortedVector(initialCapacity, IsLess{})
	{}

	TSortedVector(Index initialCapacity, IsLess isLess);

	/*! @brief Add a value to the vector.
	Complexity is O(N), where N is the size of the vector. Duplicated values are allowed, with the most
	recently-added value at the front.
	@return Current index to the newly-added value.
	*/
	Index addValue(ValueType value);

	/*! @brief Add a unique integer value to the vector.
	Similar to addValue(IntType), except that duplicated `value`s are disallowed.
	@return Current index to the newly-added value. Empty if the value exists already.
	*/
	std::optional<Index> addUniqueValue(ValueType value);

	/*! @brief Remove a value from the vector.
	Complexity is O(N), where N is the size of the vector.
	@return The index of the just-removed value. If there are more than 1 @p values, the returned
	index will be the first one. Empty if the value does not exist.
	*/
	std::optional<Index> removeValue(const ValueType& value);

	/*! @brief Remove all values that are equal to @p intValue from the vector.
	Complexity is O(N), where N is the size of the vector.
	@return The index of the just-removed value. If there are more than 1 @p values, the returned
	index will be the first one. Empty if the value does not exist.
	*/
	std::optional<Index> removeValues(const ValueType& value, Index* out_numRemovedValues = nullptr);

	/*! @brief Similar to removeValue(const Value&).
	@param index The index of the value.
	@return The just-removed value.
	*/
	ValueType removeValueByIndex(Index index);

	/*! @brief Get a value from the vector using its index.
	Complexity is O(1).
	@param index Index of the value. Note that the index for each value might change after certain
	operations. Generally it is only guaranteed to be the same if no non-const operation has been
	performed after adding the value.
	@return The value on the specified index.
	*/
	const ValueType& get(Index index) const;

	/*! @brief Find the index of a value.
	Complexity is O(logN), where N is the size of the vector.
	@return The index of the specified value. If there are more than 1 @p values, the returned
	index will be the first one. Empty if the value does not exist.
	*/
	std::optional<Index> indexOfValue(const ValueType& value) const;

	/*! @brief Check the existence of a value.
	Behaves similarly to indexOf(Value). If the index of the value is of interest, use of
	indexOfValue(Value) is recommended for efficiency (avoid finding the value twice).
	@return `true` if the speficied value exists. `false` otherwise.
	*/
	bool hasValue(const ValueType& value) const;

	/*! @brief Check how many stored values are equal to @p value.
	Complexity is O(logN + K), where N is the size of the vector and K is
	the return value.
	@return Number of values in the vector that are equal to @p value.
	*/
	Index numValues(const ValueType& value) const;

	/*! @brief Same as `std::lower_bound()`.
	Find the index to the first value x that satisfies x >= `inputValue`.
	*/
	Index lowerBound(const ValueType& inputValue) const;

	/*! @brief Get the number of values in the vector.
	*/
	Index size() const;

	/*! @brief Get the capacity of the vector.
	*/
	Index capacity() const;

	/*! @brief Check whether the size of the vector is 0.
	*/
	bool isEmpty() const;

	void shrinkToFit();

	/*! @name Iterators
	Iterators for stored values.
	*/
	///@{
	typename std::vector<ValueType>::const_iterator begin() const noexcept;
	typename std::vector<ValueType>::const_iterator end() const noexcept;
	///@}

private:
	/*!
	Find the index to the first value x that satisfies x >= `targetValue`.
	*/
	Index binarySearchSmallestInsertionIndex(const ValueType& targetValue) const;

	/*!
	Count how many values are equal to `targetValue` starting from `startingIndex`.
	*/
	Index numIdenticalValuesFrom(Index startingIndex, const ValueType& targetValue) const;

	std::vector<ValueType> m_sortedValues;

	[[PH_NO_UNIQUE_ADDRESS]] IsLess m_isLess;
};

}// end namespace ph

#include "Engine/Utility/TSortedVector.ipp"
