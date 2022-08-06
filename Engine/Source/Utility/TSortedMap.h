#pragma once

#include "Utility/TSortedVector.h"
#include "Utility/utility.h"

#include <concepts>
#include <vector>
#include <optional>
#include <cstddef>
#include <concepts>
#include <functional>
#include <utility>

namespace ph
{

/*! @brief A sorted container that offers fast value lookup & iteration, while modifications are slow.
The container has continuous memory allocation. The values are stored in ascending order according to 
their keys.
*/
template<typename KeyType, typename ValueType, typename IsLess = std::less<KeyType>>
class TSortedMap final
{
public:
	inline static constexpr std::size_t DEFAULT_INITIAL_CAPACITY = 16;

	inline TSortedMap() requires std::default_initializable<IsLess>
		: TSortedMap(DEFAULT_INITIAL_CAPACITY, IsLess{})
	{}

	inline explicit TSortedMap(const std::size_t initialCapacity) requires std::default_initializable<IsLess>
		: TSortedMap(initialCapacity, IsLess{})
	{}

	TSortedMap(std::size_t initialCapacity, IsLess isLess);

	/*! @brief Map a key to a value.
	Complexity is O(N), where N is the size of the map. Duplicated keys are allowed (i.e., one key can
	map to multiple values), with the most recently-added value at the front.
	@return Current index to the newly-added value.
	*/
	std::size_t map(KeyType key, ValueType value);

	/*! @brief Map a key just to a single value.
	Similar to map(KeyType, ValueType), except that duplicated keys are disallowed.
	@return Current index to the newly-added value. Empty if the key is mapped already.
	*/
	std::optional<std::size_t> mapUnique(KeyType key, ValueType value);

	/*! @brief Remove the value mapped to the key from the vector.
	Complexity is O(N), where N is the size of the map. If there are more than 1
	values mapped to @p key, only the first one will be removed.
	@return `true` if the key-value pair is found and removed, `false` otherwise.
	*/
	bool unmap(const KeyType& key);

	/*! @brief Remove all values mapped to the key from the vector.
	Complexity is O(N), where N is the size of the map.
	@return `true` if any key-value pair is found and removed, `false` otherwise.
	*/
	bool unmapValues(const KeyType& key);

	/*! @brief Similar to unmap(KeyType).
	@param index The index of the value.
	*/
	void unmapByIndex(std::size_t valueIndex);

	/*! @brief Get a value from the map using its associated key.
	Complexity is O(logN), where N is the size of the map.
	@return The value associated with the key. If there are more than 1 values mapped to the key, 
	the pointer to the first one will be returned. Empty if no mapped value found.
	*/
	///@{
	const ValueType* getValue(const KeyType& key) const;
	inline decltype(auto) getValue(const KeyType& key) { return mutable_cast(std::as_const(*this).getValue(key)); }
	///@}

	/*! @brief Similar to getValue(KeyType).
	Additionally, it is possible to select the desired value if there are duplicates.
	@param ithValue The value offset amount. 0 for the first, 1 for the second, etc.
	@return The value on the specified index. If there are more than 1 values mapped to the key, the
	@p ithValue one will be returned. Empty if no mapped value found or @p ithValue exceeds all duplicated values.
	*/
	///@{
	const ValueType* getValue(const KeyType& key, std::size_t ithValue) const;
	inline decltype(auto) getValue(const KeyType& key, std::size_t ithValue) { return mutable_cast(std::as_const(*this).getValue(key, ithValue)); }
	///@}

	/*! @brief Get a value from the map using its index.
	Complexity is O(1).
	@return The value on the specified index.
	*/
	///@{
	const ValueType& get(std::size_t valueIndex) const;
	ValueType& get(std::size_t valueIndex);
	///@}

	/*! @brief Get a key-value pair from the map using value index.
	Complexity is O(1).
	@return The key-value pair on the specified index, where the first element is key and the second
	element is value.
	*/
	///@{
	std::pair<const KeyType&, const ValueType&> getKeyAndValue(std::size_t valueIndex) const;
	std::pair<const KeyType&, ValueType&> getKeyAndValue(std::size_t valueIndex);
	///@}

	/*! @brief Check how many stored values are mapped to the key.
	Complexity is O(logN + K), where N is the size of the map and K is
	the return value.
	@return Number of values mapped to @p key.
	*/
	std::size_t numValues(const KeyType& key) const;

	/*! @brief Get the number of values in the map.
	*/
	std::size_t size() const;

	/*! @brief Check whether the size of the map is 0.
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
	TSortedVector<KeyType> m_keys;
	std::vector<ValueType> m_values;
};

}// end namespace ph

#include "Utility/TSortedMap.ipp"
