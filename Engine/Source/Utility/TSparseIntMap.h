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

	std::size_t map(KeyType key, ValueType value);

	// TODO how to return the value if not mapped?
	std::optional<std::size_t> mapUnique(KeyType key, ValueType value);

	std::optional<ValueType> unmap(KeyType key);

	ValueType* getValue(KeyType key) const;

	std::size_t size() const;

private:
	TSparseIntVector<KeyType> m_keys;
	std::vector<ValueType>    m_values;
};

}// end namespace ph
