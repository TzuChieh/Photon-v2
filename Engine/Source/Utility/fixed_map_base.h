#pragma once

#include <utility>
#include <tuple>
#include <type_traits>

namespace ph
{

template<typename KeyType, KeyType KEY_VAL, typename ValueType>
class TFixedMapEntry final
{
public:
	using Key   = KeyType;
	using Value = ValueType;

	static constexpr Key KEY = KEY_VAL;

	Value&       getValue()       { return m_value; }
	const Value& getValue() const { return m_value; }

private:
	Value m_value;
};

// TODO: ensure all keys are the same type, and entries are all TFixedMapEntry

template<typename... Entries>
class TFixedMapBase final
{
public:
	using EntryArray = std::tuple<Entries...>;

	static constexpr std::size_t ENTRY_ARRAY_SIZE = std::tuple_size_v<EntryArray>;
	static_assert(ENTRY_ARRAY_SIZE > 0);

	using Key = typename std::tuple_element_t<0, EntryArray>::Key;

	template<Key KEY>
	using Entry = std::tuple_element_t<INDEX, EntryArray>;

	TFixedMapBase() = default;

	TFixedMapBase(Entries&&... entries) :
		m_entries(std::move(entries)...)
	{}

	template<Key KEY, std::size_t INDEX = 0>
	static constexpr bool hasKey()
	{
		if constexpr(INDEX == ENTRY_ARRAY_SIZE)
		{
			return false;
		}

		if constexpr(std::tuple_element_t<INDEX, EntryArray>::KEY == KEY)
		{
			return true;
		}
		else
		{
			return hasKey<KEY, INDEX + 1>();
		}
	}

	template<Key KEY, std::size_t INDEX = 0>
	decltype(auto) get()
	{
		static_assert(hasKey<KEY>());

		if constexpr(std::tuple_element_t<INDEX, EntryArray>::KEY == KEY)
		{
			return std::get<INDEX>(m_entries).getValue();
		}
		else
		{
			return get<KEY, INDEX + 1>();
		}
	}

	template<Key KEY, std::size_t INDEX = 0>
	decltype(auto) get() const
	{
		static_assert(hasKey<KEY>());

		if constexpr(std::tuple_element_t<INDEX, EntryArray>::KEY == KEY)
		{
			return std::get<INDEX>(m_entries).getValue();
		}
		else
		{
			return get<KEY, INDEX + 1>();
		}
	}

	template<Key KEY, std::size_t INDEX = 0>
	static constexpr std::size_t entryIndex()
	{
		static_assert(hasKey<KEY>());

		if constexpr(std::tuple_element_t<INDEX, EntryArray>::KEY == KEY)
		{
			return INDEX;
		}
		else
		{
			return entryIndex<KEY, INDEX + 1>();
		}
	}

private:
	EntryArray m_entries;
};

}// end namespace ph