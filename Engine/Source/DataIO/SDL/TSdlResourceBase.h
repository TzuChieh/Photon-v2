#pragma once

#include "DataIO/SDL/ISdlResource.h"
#include "DataIO/SDL/SdlResourceId.h"
#include "DataIO/SDL/ETypeCategory.h"
#include "Common/assertion.h"

namespace ph
{

/*! @brief A convenient ISdlResource with core requirements implementationed.
*/
template<ETypeCategory TYPE_CATEGORY>
class TSdlResourceBase : public ISdlResource
{
public:
	/*! @brief Static category information of the resource.
	This static category information is required by ISdlResource. See its documentation
	for more details.
	*/
	static constexpr ETypeCategory CATEGORY = TYPE_CATEGORY;

protected:
	TSdlResourceBase();
	TSdlResourceBase(const TSdlResourceBase& other);
	inline TSdlResourceBase(TSdlResourceBase&& other) = default;

	TSdlResourceBase& operator = (const TSdlResourceBase& rhs);
	inline TSdlResourceBase& operator = (TSdlResourceBase&& rhs) = default;

public:
	ETypeCategory getCategory() const override;

	SdlResourceId getId() const override;

private:
	SdlResourceId m_resourceId;
};

// In-header Implementations:

template<ETypeCategory TYPE_CATEGORY>
inline TSdlResourceBase<TYPE_CATEGORY>::TSdlResourceBase() :

	ISdlResource(),

	m_resourceId(gen_sdl_resource_id())
{
	PH_ASSERT_NE(m_resourceId, EMPTY_SDL_RESOURCE_ID);
}

template<ETypeCategory TYPE_CATEGORY>
inline TSdlResourceBase<TYPE_CATEGORY>::TSdlResourceBase(const TSdlResourceBase& other) :
	
	ISdlResource(other),

	// ID should not be copied--it must be re-generated
	m_resourceId(gen_sdl_resource_id())
{
	PH_ASSERT_NE(m_resourceId, EMPTY_SDL_RESOURCE_ID);
}

template<ETypeCategory TYPE_CATEGORY>
inline ETypeCategory TSdlResourceBase<TYPE_CATEGORY>::getCategory() const
{
	return CATEGORY;
}

template<ETypeCategory TYPE_CATEGORY>
inline SdlResourceId TSdlResourceBase<TYPE_CATEGORY>::getId() const
{
	return m_resourceId;
}

template<ETypeCategory TYPE_CATEGORY>
inline TSdlResourceBase<TYPE_CATEGORY>& TSdlResourceBase<TYPE_CATEGORY>::operator = (const TSdlResourceBase& rhs)
{
	ISdlResource::operator = (rhs);

	// Note: m_resourceId is left unchanged--no need to re-generate as it should be done in ctors

	return *this;
}

}// end namespace ph
