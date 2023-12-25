#pragma once

#include "SDL/ISdlResource.h"
#include "SDL/SdlResourceId.h"
#include "SDL/ESdlTypeCategory.h"

#include <Common/assertion.h>

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

private:
	SdlResourceId m_resourceId;
};

// In-header Implementations:

inline SdlResourceBase::SdlResourceBase() :

	ISdlResource(),

	m_resourceId(gen_sdl_resource_id())
{
	PH_ASSERT_NE(m_resourceId, EMPTY_SDL_RESOURCE_ID);
}

inline SdlResourceBase::SdlResourceBase(const SdlResourceBase& other) :
	
	ISdlResource(other),

	// ID should not be copied--it must be re-generated
	m_resourceId(gen_sdl_resource_id())
{
	PH_ASSERT_NE(m_resourceId, EMPTY_SDL_RESOURCE_ID);
}

inline SdlResourceId SdlResourceBase::getId() const
{
	return m_resourceId;
}

inline SdlResourceBase& SdlResourceBase::operator = (const SdlResourceBase& rhs)
{
	ISdlResource::operator = (rhs);

	// Note: m_resourceId is left unchanged--no need to re-generate as it should be done in ctors

	return *this;
}

}// end namespace ph
