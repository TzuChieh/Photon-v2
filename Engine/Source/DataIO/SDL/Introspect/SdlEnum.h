#pragma once

#include <string_view>
#include <cstddef>
#include <utility>

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
			name(), value(0)
		{}
	};

	using Entry = TEntry<std::uintmax_t>;

	explicit SdlEnum(std::string name);

	virtual Entry getEntry(std::size_t entryIndex) const = 0;
	virtual std::size_t numEntries() const = 0;

	const std::string& getName() const;
	const std::string& getDescription() const;

protected:
	SdlEnum& setDescription(std::string description);

private:
	std::string m_name;
	std::string m_description;
};

// In-header Implementations:

inline SdlEnum::SdlEnum(std::string name) : 
	m_name(std::move(name))
{
	PH_ASSERT(!name.empty());
}

inline SdlEnum& SdlEnum::setDescription(std::string description)
{
	m_description = std::move(description);

	return *this;
}

inline const std::string& SdlEnum::getName() const
{
	return m_name;
}

inline const std::string& SdlEnum::getDescription() const
{
	return m_description;
}

}// end namespace ph
