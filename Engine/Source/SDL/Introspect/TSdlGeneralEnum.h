#pragma once

#include "SDL/Introspect/SdlEnum.h"
#include "Utility/TArrayAsVector.h"
#include "Common/assertion.h"
#include "SDL/sdl_exceptions.h"
#include "Utility/utility.h"

#include <type_traits>
#include <string>
#include <string_view>
#include <cstddef>
#include <utility>

namespace ph
{

/*! @brief SDL enum implementation with common features.
Enum value and string mapping are done in a brute-force way.
*/
template<typename EnumType_, std::size_t MAX_ENTRIES = 64>
class TSdlGeneralEnum : public SdlEnum
{
	// TODO: how the mapping is done should be a template param
	// TODO: can have other kinds of enum such as TSdlFlagEnum
public:
	using EnumType = EnumType_;

	static_assert(std::is_enum_v<EnumType>,
		"EnumType must be a C++ enum. Currently it is not.");

public:
	inline explicit TSdlGeneralEnum(std::string name) :

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
		return m_entries.size();
	}

	inline TSdlGeneralEnum& addEntry(
		const EnumType         enumValue, 
		const std::string_view valueName,
		std::string            description = "")
	{
		PH_ASSERT_MSG(!m_entries.isFull(),
			"No space for more entries; increase MAX_ENTRIES parameter for this enum.");

		BasicEnumEntry entry;
		entry.nameIndex = m_nameBuffer.size();
		entry.nameSize  = valueName.size();
		entry.value     = enumValue;

		m_nameBuffer.append(valueName);
		PH_ASSERT_EQ(entry.nameIndex + entry.nameSize, m_nameBuffer.size());

		const std::size_t entryIndex = m_entries.size();
		m_entries.pushBack(entry);
		setEntryDescription(entryIndex, std::move(description));

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

	/*! @brief Get an enum entry via an enum name.

	Note that the method cannot distinguish between identical enum names,
	i.e., if two entries have the same enum name, their entries cannot be
	uniquely identified and returned.
	*/
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

		throw SdlLoadError("use of invalid enum entry name <" + std::string(entryName) + ">");
	}

	/*! @brief Get an enum entry via an enum value.

	Note that the method cannot distinguish between identical enum values,
	i.e., if two entries have the same enum value, their entries cannot be
	uniquely identified and returned.
	*/
	inline TEntry<EnumType> getTypedEntry(const EnumType enumValue) const
	{
		// Brute-force search for matched enum entry value
		for(std::size_t entryIdx = 0; entryIdx < m_entries.size(); ++entryIdx)
		{
			const TEntry<EnumType> entry = getTypedEntry(entryIdx);
			if(entry.value == enumValue)
			{
				return entry;
			}
		}

		throw SdlLoadError("use of invalid enum value: " + enum_to_string(enumValue));
	}

	inline TSdlGeneralEnum& description(std::string descriptionStr)
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
			nameIndex(0), nameSize(0), value(static_cast<EnumType>(0))
		{}
	};

	std::string m_nameBuffer;
	TArrayAsVector<BasicEnumEntry, MAX_ENTRIES> m_entries;
};

}// end namespace ph
