#pragma once

#include "Engine/SDL/Definition/SdlUserSpec.h"

#include <Common/primitive_type.h>

#include <string>
#include <string_view>
#include <cstddef>
#include <vector>

namespace ph
{

/*! @brief Describes enum in SDL.

Note that it is highly encouraged to follow several conventions while using
SDL enum:
- Define an entry with value = 0 to indicate a default/unspecified state.
- Define only unique enum entries--no identical enum values or enum names.
*/
class SdlEnum
{
public:
	template<typename ValueType>
	struct TEntry
	{
		std::string_view name;
		ValueType        value;

		inline TEntry() :
			name(), value(static_cast<ValueType>(0))
		{}

		inline TEntry(std::string_view name, ValueType value) :
			name(std::move(name)), value(std::move(value))
		{}
	};

	using Entry = TEntry<int64>;

	SdlEnum();

	virtual Entry getEntry(std::size_t entryIndex) const = 0;
	virtual std::size_t numEntries() const = 0;

	const std::string& getName() const;
	const std::string& getDescription() const;
	std::string getEntryDescription(std::size_t entryIndex) const;
	const SdlUserSpec& getUserSpec() const;

protected:
	SdlEnum& setName(std::string name);
	SdlEnum& setDescription(std::string description);
	SdlEnum& setEntryDescription(std::size_t entryIndex, std::string description);
	SdlEnum& setUserSpec(SdlUserSpec userSpec);

private:
	std::string              m_name;
	std::string              m_description;
	std::vector<std::string> m_entryDescriptions;
	SdlUserSpec              m_userSpec;
};

// In-header Implementations:

inline const std::string& SdlEnum::getName() const
{
	return m_name;
}

inline const std::string& SdlEnum::getDescription() const
{
	return m_description;
}

// Returns a copy of string as the method accepts out-of-bound entry index
inline std::string SdlEnum::getEntryDescription(const std::size_t entryIndex) const
{
	return entryIndex < m_entryDescriptions.size() ? 
		m_entryDescriptions[entryIndex] : "";
}

inline const SdlUserSpec& SdlEnum::getUserSpec() const
{
	return m_userSpec;
}

}// end namespace ph
