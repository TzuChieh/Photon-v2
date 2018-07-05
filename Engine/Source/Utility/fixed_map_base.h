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

template<typename... Entries>
class TFixedMapBase final
{
public:
	using EntryArray = std::tuple<Entries...>;

	static constexpr std::size_t ENTRY_ARRAY_SIZE = std::tuple_size_v<EntryArray>;
	static_assert(ENTRY_ARRAY_SIZE > 0);

	using Key = typename std::tuple_element_t<0, EntryArray>::Key;

	TFixedMapBase() = default;

	TFixedMapBase(Entries&&... entries) :
		m_entries(entries...)
	{}

	template<Key KEY, std::size_t INDEX = 0>
	decltype(auto) get()
	{
		static_assert(INDEX < ENTRY_ARRAY_SIZE);

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
		static_assert(INDEX < ENTRY_ARRAY_SIZE);

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
	std::size_t getEntryIndex() const
	{
		static_assert(INDEX < ENTRY_ARRAY_SIZE);

		if constexpr(std::tuple_element_t<INDEX, EntryArray>::KEY == KEY)
		{
			return INDEX;
		}
		else
		{
			return getEntryIndex<KEY, INDEX + 1>();
		}
	}

private:
	EntryArray m_entries;
};

}// end namespace ph