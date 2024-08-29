#pragma once

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
template<typename ValueType, typename IsLess = std::less<ValueType>>
class TSortedVector final
{
public:
	inline TSortedVector() requires std::default_initializable<IsLess>
		: TSortedVector(0, IsLess{})
	{}

	inline explicit TSortedVector(const std::size_t initialCapacity) requires std::default_initializable<IsLess>
		: TSortedVector(initialCapacity, IsLess{})
	{}

	TSortedVector(std::size_t initialCapacity, IsLess isLess);

	/*! @brief Add a value to the vector.
	Complexity is O(N), where N is the size of the vector. Duplicated values are allowed, with the most
	recently-added value at the front.
	@return Current index to the newly-added value.
	*/
	std::size_t addValue(ValueType value);

	/*! @brief Add a unique integer value to the vector.
	Similar to addValue(IntType), except that duplicated `value`s are disallowed.
	@return Current index to the newly-added value. Empty if the value exists already.
	*/
	std::optional<std::size_t> addUniqueValue(ValueType value);

	/*! @brief Remove a value from the vector.
	Complexity is O(N), where N is the size of the vector.
	@return The index of the just-removed value. If there are more than 1 @p values, the returned
	index will be the first one. Empty if the value does not exist.
	*/
	std::optional<std::size_t> removeValue(const ValueType& value);

	/*! @brief Remove all values that are equal to @p intValue from the vector.
	Complexity is O(N), where N is the size of the vector.
	@return The index of the just-removed value. If there are more than 1 @p values, the returned
	index will be the first one. Empty if the value does not exist.
	*/
	std::optional<std::size_t> removeValues(const ValueType& value, std::size_t* out_numRemovedValues = nullptr);

	/*! @brief Similar to removeValue(const Value&).
	@param index The index of the value.
	@return The just-removed value.
	*/
	ValueType removeValueByIndex(std::size_t index);

	/*! @brief Get a value from the vector using its index.
	Complexity is O(1).
	@param index Index of the value. Note that the index for each value might change after certain
	operations. Generally it is only guaranteed to be the same if no non-const operation has been
	performed after adding the value.
	@return The value on the specified index.
	*/
	const ValueType& get(std::size_t index) const;

	/*! @brief Find the index of a value.
	Complexity is O(logN), where N is the size of the vector.
	@return The index of the specified value. If there are more than 1 @p values, the returned
	index will be the first one. Empty if the value does not exist.
	*/
	std::optional<std::size_t> indexOfValue(const ValueType& value) const;

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
	std::size_t numValues(const ValueType& value) const;

	/*! @brief Get the number of values in the vector.
	*/
	std::size_t size() const;

	/*! @brief Check whether the size of the vector is 0.
	*/
	bool isEmpty() const;

	/*! @name Iterators
	Iterators for stored values.
	*/
	///@{
	typename std::vector<ValueType>::const_iterator begin() const noexcept;
	typename std::vector<ValueType>::const_iterator end() const noexcept;
	///@}

private:
	std::vector<ValueType> m_sortedValues;

	[[no_unique_address]] IsLess m_isLess;

	/*!
	Find the index to the first value x that satisfies x >= `targetValue`.
	*/
	std::size_t binarySearchSmallestInsertionIndex(const ValueType& targetValue) const;

	/*!
	Count how many values are equal to `targetValue` starting from `startingIndex`.
	*/
	std::size_t numIdenticalValuesFrom(std::size_t startingIndex, const ValueType& targetValue) const;
};

}// end namespace ph

#include "Utility/TSortedVector.ipp"
