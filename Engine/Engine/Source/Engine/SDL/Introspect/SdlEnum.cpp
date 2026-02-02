#include "Engine/SDL/Introspect/SdlEnum.h"

#include <Common/assertion.h>

#include <utility>

namespace ph
{

SdlEnum::SdlEnum()
	: m_name()
	, m_description()
	, m_entryDescriptions()
	, m_userSpec()
{}

SdlEnum& SdlEnum::setName(std::string name)
{
	m_name = std::move(name);

	return *this;
}

SdlEnum& SdlEnum::setDescription(std::string description)
{
	m_description = std::move(description);

	return *this;
}

SdlEnum& SdlEnum::setEntryDescription(const std::size_t entryIndex, std::string description)
{
	// Allocate more storage for entry descriptions if required
	if(entryIndex >= m_entryDescriptions.size())
	{
		m_entryDescriptions.resize(entryIndex + 1);
	}

	PH_ASSERT_LT(entryIndex, m_entryDescriptions.size());
	m_entryDescriptions[entryIndex] = std::move(description);

	return *this;
}

SdlEnum& SdlEnum::setUserSpec(SdlUserSpec userSpec)
{
	m_userSpec = std::move(userSpec);
	return *this;
}

}// end namespace ph
