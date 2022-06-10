#pragma once

#include <concepts>
#include <vector>
#include <cstddef>
#include <optional>

namespace ph
{

template<std::integral IntType>
class TSparseIntVector final
{
public:
	TSparseIntVector();
	explicit TSparseIntVector(std::size_t initialCapacity);

	// TODO: bulk add
	// TODO: iterators (for each)

	/*! @brief Add an integer value to the vector.
	Complexity is $ O \left( N \right) $, where N is the size of the vector. Duplicated values are allowed.
	@return Index to the newly-added value.
	*/
	std::size_t addValue(IntType intValue);

	/*! @brief Add a unique integer value to the vector.
	Similar to  addValue(IntType), except that duplicated values are disallowed.
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

	/*! @brief Get an integer value from the vector using its index.
	Complexity is $ O \left( 1 \right) $.
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

private:
	std::vector<IntType> m_intValues;
};

}// end namespace ph

#include "Utility/TSparseIntVector.ipp"
