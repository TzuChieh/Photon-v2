#pragma once

#include "Engine/SDL/ISdlResource.h"
#include "Engine/SDL/SdlResourceId.h"
#include "Engine/SDL/ESdlTypeCategory.h"
#include "Engine/SDL/Introspect/TSdlString.h"

#include <Common/assertion.h>

#include <string>
#include <string_view>
#include <utility>

namespace ph
{

/*! @brief A convenient ISdlResource with some core requirements implemented.
This class does not provide static/dynamic category information. The work is left
for derived classes. See ISdlResource for more implementation requirements.
*/
class SdlResourceBase : public ISdlResource
{
protected:
	SdlResourceBase();
	SdlResourceBase(const SdlResourceBase& other);
	SdlResourceBase(SdlResourceBase&& other) = default;

	SdlResourceBase& operator = (const SdlResourceBase& rhs);
	SdlResourceBase& operator = (SdlResourceBase&& rhs) = default;

public:
	ESdlTypeCategory getDynamicCategory() const override = 0;

	SdlResourceId getId() const override;
	std::string_view getDisplayName() const override;
	void setDisplayName(std::string displayName) override;

protected:
	template<typename Owner>
	static TSdlString<Owner> makeDisplayNameField();

private:
	SdlResourceId m_resourceId;
	std::string m_displayName;
};

// In-header Implementations:

inline SdlResourceBase::SdlResourceBase() :

	ISdlResource(),

	m_resourceId(gen_sdl_resource_id()),
	m_displayName()
{
	PH_ASSERT_NE(m_resourceId, EMPTY_SDL_RESOURCE_ID);
}

inline SdlResourceBase::SdlResourceBase(const SdlResourceBase& other) :
	
	ISdlResource(other),

	// ID should not be copied--it must be re-generated
	m_resourceId(gen_sdl_resource_id()),
	m_displayName(other.m_displayName)
{
	PH_ASSERT_NE(m_resourceId, EMPTY_SDL_RESOURCE_ID);
}

inline SdlResourceId SdlResourceBase::getId() const
{
	return m_resourceId;
}

inline std::string_view SdlResourceBase::getDisplayName() const
{
	return m_displayName;
}

inline void SdlResourceBase::setDisplayName(std::string displayName)
{
	m_displayName = std::move(displayName);
}

inline SdlResourceBase& SdlResourceBase::operator = (const SdlResourceBase& rhs)
{
	ISdlResource::operator = (rhs);

	m_displayName = rhs.m_displayName;

	// Note: `m_resourceId` is left unchanged--no need to re-generate as it should be done in ctors

	return *this;
}

template<typename Owner>
inline TSdlString<Owner> SdlResourceBase::makeDisplayNameField()
{
	TSdlString<Owner> displayName("display-name", &SdlResourceBase::m_displayName);
	displayName.description("Human-readable name for UI, editor, and debugging.");
	displayName.defaultTo("");
	displayName.optional();
	return displayName;
}

}// end namespace ph
