#include "World/Foundation/ResourceCookReport.h"
#include "Common/assertion.h"

namespace ph
{

ResourceCookReport::ResourceCookReport(SdlResourceId rawResourceId)
	: m_rawResourceId(rawResourceId)
	, m_isCookable   (true)
{
	PH_ASSERT_NE(rawResourceId, EMPTY_SDL_RESOURCE_ID);
}

void ResourceCookReport::setIsCookable(const bool isCookable)
{
	m_isCookable = isCookable;
}

bool ResourceCookReport::isCookable() const
{
	return m_isCookable;
}

}// end namespace ph
