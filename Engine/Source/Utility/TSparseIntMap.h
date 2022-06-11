#pragma once

#include "Utility/TSparseIntVector.h"

#include <concepts>
#include <vector>
#include <optional>
#include <cstddef>

namespace ph
{

template<std::integral KeyType, typename ValueType>
class TSparseIntMap final
{
public:
	TSparseIntMap();
	explicit TSparseIntMap(std::size_t initialCapacity);

	/*! @brief Map an integer key to a value.
	Complexity is $ O \left( N \right) $, where N is the size of the map. Duplicated keys are allowed
	(i.e., one key can map to multiple values).
	@return Index to the newly-added value.
	*/
	std::size_t map(KeyType key, ValueType value);

	/*! @brief Map an integer key just to a single value.
	Similar to map(KeyType, ValueType), except that duplicated keys are disallowed.
	@return Index to the newly-added value. Empty if the key is mapped already.
	*/
	std::optional<std::size_t> mapUnique(KeyType key, ValueType value);

	/*! @brief Remove an integer-value pair from the vector.
	Complexity is $ O \left( N \right) $, where N is the size of the map.
	@return The index of the just-removed value. If there are more than 1 @p intValue, the returned
	index will be the first one. Empty if the value does not exist.
	*/
	std::optional<ValueType> unmap(KeyType key);

	/*! @brief Get a value from the map using its associated integer key.
	Complexity is $ O \left( logN \right) $, where N is the size of the map.
	@return The value on the specified index. If there are more than 1 values mapped to the key, the first
	one will be returned. Empty if no mapped value found.
	*/
	ValueType* getValue(KeyType key) const;

	/*! @brief Get a value from the map using its associated integer key.
	Similar to getValue(KeyType) except that it is possible to select the desired value if there
	are duplicates.
	@param ithValue The value offset amount. 0 for the first, 1 for the second, etc.
	@return The value on the specified index. If there are more than 1 values mapped to the key, the
	@p ithValue one will be returned. Empty if no mapped value found or @p exceeds all duplicated values.
	*/
	ValueType* getValue(KeyType key, std::size_t ithValue) const;

	/*! @brief Get a value from the map using its index.
	Complexity is $ O \left( 1 \right) $.
	@return The value on the specified index.
	*/
	ValueType& get(std::size_t index) const;

	/*! @brief Get the number of values in the map.
	*/
	std::size_t size() const;

private:
	TSparseIntVector<KeyType> m_keys;
	std::vector<ValueType>    m_values;
};

}// end namespace ph
