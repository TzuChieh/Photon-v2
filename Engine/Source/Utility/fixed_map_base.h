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

	template<std::size_t ENTRY_INDEX>
	using Entry = std::tuple_element_t<ENTRY_INDEX, EntryArray>;

	TFixedMapBase() = default;

	TFixedMapBase(TFixedMapBase&& other) = default;

	TFixedMapBase(Entries&&... entries) :
		m_entries(std::move(entries)...)
	{}

	template<Key KEY, std::size_t D_ENTRY_INDEX = 0>
	static constexpr bool hasKey()
	{
		if constexpr(D_ENTRY_INDEX == ENTRY_ARRAY_SIZE)
		{
			return false;
		}

		if constexpr(Entry<D_ENTRY_INDEX>::KEY == KEY)
		{
			return true;
		}
		else
		{
			return hasKey<KEY, D_ENTRY_INDEX + 1>();
		}
	}

	template<std::size_t ENTRY_INDEX>
	static constexpr Key entryKey()
	{
		static_assert(ENTRY_INDEX < ENTRY_ARRAY_SIZE);

		return Entry<ENTRY_INDEX>::KEY;
	}

	template<Key KEY, std::size_t D_ENTRY_INDEX = 0>
	static constexpr std::size_t entryIndex()
	{
		static_assert(hasKey<KEY>());

		if constexpr(Entry<D_ENTRY_INDEX>::KEY == KEY)
		{
			return D_ENTRY_INDEX;
		}
		else
		{
			return entryIndex<KEY, D_ENTRY_INDEX + 1>();
		}
	}

	template<std::size_t ENTRY_INDEX>
	decltype(auto) getEntry()
	{
		static_assert(ENTRY_INDEX < ENTRY_ARRAY_SIZE);

		return std::get<ENTRY_INDEX>(m_entries);
	}

	template<std::size_t ENTRY_INDEX>
	decltype(auto) getEntry() const
	{
		static_assert(ENTRY_INDEX < ENTRY_ARRAY_SIZE);

		return std::get<ENTRY_INDEX>(m_entries);
	}

	template<Key KEY>
	decltype(auto) get()
	{
		return getEntry<entryIndex<KEY>()>().getValue();
	}

	template<Key KEY>
	decltype(auto) get() const
	{
		return getEntry<entryIndex<KEY>()>().getValue();
	}

	TFixedMapBase& operator = (TFixedMapBase&& rhs) = default;

private:
	EntryArray m_entries;
};

}// end namespace ph