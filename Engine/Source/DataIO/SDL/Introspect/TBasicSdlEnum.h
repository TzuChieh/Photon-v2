#pragma once

#include "DataIO/SDL/Introspect/SdlEnum.h"
#include "Utility/TArrayAsVector.h"
#include "Common/assertion.h"

#include <type_traits.h>
#include <string>
#include <cstddef>
#include <utility>

namespace ph
{

template<typename EnumType_, auto NUM_ENTRIES_>
class TBasicSdlEnum : public SdlEnum
{
public:
	using EnumType = EnumType_;

	constexpr std::size_t NUM_ENTRIES = static_cast<std::size_t>(NUM_ENTRIES_);

	static_assert(std::is_enum_v<EnumType>,
		"EnumType must be a C++ enum. Currently it is not.");

public:
	inline explicit TBasicSdlEnum(std::string name) :

		SdlEnum(std::move(name)),

		m_nameBuffer(),
		m_entries()
	{}

	inline Entry getEntry(const std::size_t entryIndex) const override
	{
		using EntryValueType = decltype(std::declval<Entry>().value);

		const TEntry<EnumType> entry = getTypedEntry(entryIndex);
		return {entry.name, static_cast<EntryValueType>(entry.value)};
	}

	inline std::size_t numEntries() const override
	{
		return NUM_ENTRIES;
	}

	inline TBasicSdlEnum& addEntry(const Enum enumValue, const std::string_view valueName)
	{
		BasicEnumEntry entry;
		entry.nameIndex = m_nameBuffer.size();
		entry.nameSize  = valueName.size();
		entry.value     = enumValue;

		m_nameBuffer.append(valueName);
		PH_ASSERT_EQ(entry.nameIndex + entry.nameSize, m_nameBuffer.size());

		m_entries.pushBack(entry);

		return *this;
	}

	inline TEntry<EnumType> getTypedEntry(const std::size_t entryIndex) const
	{
		const BasicEnumEntry& basicEntry = m_entries[entryIndex];

		const std::string_view entryName(
			m_nameBuffer.data() + basicEntry.nameIndex,
			basicEntry.nameSize);

		return {entryName, basicEntry.value};
	}

	inline TEntry<EnumType> getTypedEntry(const std::string_view entryName) const
	{
		// Brute-force search for matched enum entry name
		for(std::size_t entryIdx = 0; entryIdx < m_entries.size(); ++entryIdx)
		{
			const TEntry<EnumType> entry = getTypedEntry(entryIdx);
			if(entry.name == entryName)
			{
				return entry;
			}
		}

		return TEntry<Enum>();
	}

	inline TEntry<EnumType> getTypedEntry(const std::string_view entryName) const
	{
		// Brute-force search for matched enum entry name
		for(std::size_t entryIdx = 0; entryIdx < m_entries.size(); ++entryIdx)
		{
			const TEntry<EnumType> entry = getTypedEntry(entryIdx);
			if(entry.name == entryName)
			{
				return entry;
			}
		}

		return TEntry<Enum>();
	}

	inline TBasicSdlEnum& description(std::string descriptionStr)
	{
		setDescription(std::move(descriptionStr));
		return *this;
	}

private:
	struct BasicEnumEntry
	{
		std::size_t nameIndex;
		std::size_t nameSize;
		EnumType    value;

		inline BasicEnumEntry() :
			nameIndex(0), nameSize(0), value(0)
		{}
	};

	std::string m_nameBuffer;
	TArrayAsVector<BasicEnumEntry, NUM_ENTRIES> m_entries;
};

}// end namespace ph
