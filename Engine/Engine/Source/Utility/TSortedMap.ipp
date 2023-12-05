#pragma once

#include "Utility/TSortedMap.h"

#include <Common/assertion.h>

namespace ph
{

/*
Values stored in the map are stored at the same index as their keys. The implementation generally
performs the same operation as those performed by `TSortedVector<KeyType>`.
*/

template<typename KeyType, typename ValueType, typename IsLess>
inline TSortedMap<KeyType, ValueType, IsLess>::TSortedMap(const std::size_t initialCapacity, IsLess isLess)
	: m_keys  (initialCapacity, std::move(isLess))
	, m_values()
{
	m_values.reserve(initialCapacity);
}

template<typename KeyType, typename ValueType, typename IsLess>
inline std::size_t TSortedMap<KeyType, ValueType, IsLess>::map(KeyType key, ValueType value)
{
	const auto keyIndex = m_keys.addValue(std::move(key));
	m_values.insert(m_values.begin() + keyIndex, std::move(value));
	return keyIndex;
}

template<typename KeyType, typename ValueType, typename IsLess>
inline std::optional<std::size_t> TSortedMap<KeyType, ValueType, IsLess>::mapUnique(KeyType key, ValueType value)
{
	const auto optKeyIndex = m_keys.addUniqueValue(std::move(key));
	if(!optKeyIndex.has_value())
	{
		return std::nullopt;
	}

	PH_ASSERT(optKeyIndex.has_value());
	m_values.insert(m_values.begin() + *optKeyIndex, std::move(value));
	return optKeyIndex;
}

template<typename KeyType, typename ValueType, typename IsLess>
inline bool TSortedMap<KeyType, ValueType, IsLess>::unmap(const KeyType& key)
{
	const auto optFirstKeyIndex = m_keys.removeValue(key);
	if(optFirstKeyIndex.has_value())
	{
		m_values.erase(m_values.begin() + *optFirstKeyIndex);
	}
	return optFirstKeyIndex.has_value();
}

template<typename KeyType, typename ValueType, typename IsLess>
inline bool TSortedMap<KeyType, ValueType, IsLess>::unmapValues(const KeyType& key)
{
	std::size_t numRemovedValues = 0;
	const auto optFirstKeyIndex = m_keys.removeValues(key, &numRemovedValues);
	if(optFirstKeyIndex.has_value())
	{
		// Handles the cases where `m_values` is empty and/or `numRemovedValues` is 0
		m_values.erase(
			m_values.begin() + *optFirstKeyIndex,
			m_values.begin() + *optFirstKeyIndex + numRemovedValues);
	}
	return optFirstKeyIndex.has_value();
}

template<typename KeyType, typename ValueType, typename IsLess>
inline void TSortedMap<KeyType, ValueType, IsLess>::unmapByIndex(const std::size_t valueIndex)
{
	PH_ASSERT_LT(valueIndex, m_keys.size());
	PH_ASSERT_LT(valueIndex, m_values.size());

	m_keys.removeValueByIndex(valueIndex);
	m_values.erase(m_values.begin() + valueIndex);
}

template<typename KeyType, typename ValueType, typename IsLess>
inline const ValueType* TSortedMap<KeyType, ValueType, IsLess>::getValue(const KeyType& key) const
{
	const auto optFirstKeyIndex = m_keys.indexOfValue(key);
	if(optFirstKeyIndex.has_value())
	{
		PH_ASSERT_LT(*optFirstKeyIndex, m_values.size());
		return &m_values[*optFirstKeyIndex];
	}
	else
	{
		return nullptr;
	}
}

template<typename KeyType, typename ValueType, typename IsLess>
inline const ValueType* TSortedMap<KeyType, ValueType, IsLess>::getValue(const KeyType& key, const std::size_t ithValue) const
{
	const auto optFirstKeyIndex = m_keys.indexOfValue(key);
	if(optFirstKeyIndex.has_value())
	{
		// We must make sure that the i-th index starting from the first key index stores a key value
		// equals to `key`, which means we are still in the range of values that are mapped to `key`.
		// Do not rely on testing the equality of values as `ValueType` may not be comparable, or 
		// values that compare equal were mapped to different keys.

		const std::size_t ithKeyIndex = *optFirstKeyIndex + ithValue;
		if(ithKeyIndex < m_keys.size() && m_keys.get(ithKeyIndex) == key)
		{
			PH_ASSERT_LT(ithKeyIndex, m_values.size());
			return &m_values[ithKeyIndex];
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
}

template<typename KeyType, typename ValueType, typename IsLess>
inline const ValueType& TSortedMap<KeyType, ValueType, IsLess>::get(const std::size_t valueIndex) const
{
	PH_ASSERT_LT(valueIndex, m_values.size());
	return m_values[valueIndex];
}

template<typename KeyType, typename ValueType, typename IsLess>
inline ValueType& TSortedMap<KeyType, ValueType, IsLess>::get(const std::size_t valueIndex)
{
	PH_ASSERT_LT(valueIndex, m_values.size());
	return m_values[valueIndex];
}

template<typename KeyType, typename ValueType, typename IsLess>
inline std::pair<const KeyType&, const ValueType&> TSortedMap<KeyType, ValueType, IsLess>::getKeyAndValue(const std::size_t valueIndex) const
{
	return {m_keys.get(valueIndex), get(valueIndex)};
}

template<typename KeyType, typename ValueType, typename IsLess>
inline std::pair<const KeyType&, ValueType&> TSortedMap<KeyType, ValueType, IsLess>::getKeyAndValue(const std::size_t valueIndex)
{
	return {m_keys.get(valueIndex), get(valueIndex)};
}

template<typename KeyType, typename ValueType, typename IsLess>
inline std::size_t TSortedMap<KeyType, ValueType, IsLess>::numValues(const KeyType& key) const
{
	// We store duplicated keys for values mapped to the same key
	const auto numIdenticalKeys = m_keys.numValues(key);
	return numIdenticalKeys;
}

template<typename KeyType, typename ValueType, typename IsLess>
inline std::size_t TSortedMap<KeyType, ValueType, IsLess>::size() const
{
	return m_values.size();
}

template<typename KeyType, typename ValueType, typename IsLess>
inline bool TSortedMap<KeyType, ValueType, IsLess>::isEmpty() const
{
	return m_values.empty();
}

template<typename KeyType, typename ValueType, typename IsLess>
inline typename std::vector<ValueType>::const_iterator TSortedMap<KeyType, ValueType, IsLess>::begin() const noexcept
{
	return m_values.begin();
}

template<typename KeyType, typename ValueType, typename IsLess>
inline typename std::vector<ValueType>::const_iterator TSortedMap<KeyType, ValueType, IsLess>::end() const noexcept
{
	return m_values.end();
}

}// end namespace ph
