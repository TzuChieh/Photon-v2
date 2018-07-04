#pragma once

#include <utility>
#include <tuple>
#include <type_traits>

namespace ph
{

namespace fixed_integer_map_detail
{
	using Key = int;
}

template<fixed_integer_map_detail::Key KEY, typename Value>
class TFixedIntegerMapEntry final
{
public:
	static constexpr fixed_integer_map_detail::Key KEY_VALUE = KEY;

	Value&       getValue()       { return m_value; }
	const Value& getValue() const { return m_value; }

private:
	Value m_value;
};

template<typename... Entries>
class TFixedIntegerMap final
{
public:
	using EntryArray = std::tuple<Entries...>;

	static constexpr std::size_t ENTRY_ARRAY_SIZE = std::tuple_size_v<EntryArray>;

	TFixedIntegerMap() = default;

	TFixedIntegerMap(Entries&&... entries) :
		m_entries(entries...)
	{}

	template<fixed_integer_map_detail::Key KEY, std::size_t INDEX = 0>
	decltype(auto) get()
	{
		static_assert(INDEX < ENTRY_ARRAY_SIZE);

		if constexpr(std::tuple_element_t<INDEX, EntryArray>::KEY_VALUE == KEY)
		{
			return std::get<INDEX>(m_entries).getValue();
		}
		else
		{
			return get<KEY, INDEX + 1>();
		}
	}

	template<fixed_integer_map_detail::Key KEY, std::size_t INDEX = 0>
	decltype(auto) get() const
	{
		static_assert(INDEX < ENTRY_ARRAY_SIZE);

		if constexpr(std::tuple_element_t<INDEX, EntryArray>::KEY_VALUE == KEY)
		{
			return std::get<INDEX>(m_entries).getValue();
		}
		else
		{
			return get<KEY, INDEX + 1>();
		}
	}

	template<fixed_integer_map_detail::Key KEY, std::size_t INDEX = 0>
	std::size_t getEntryIndex() const
	{
		static_assert(INDEX < ENTRY_ARRAY_SIZE);

		if constexpr(std::tuple_element_t<INDEX, EntryArray>::KEY_VALUE == KEY)
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