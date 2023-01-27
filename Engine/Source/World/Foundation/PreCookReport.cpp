#include "World/Foundation/PreCookReport.h"
#include "Common/assertion.h"

namespace ph
{

PreCookReport::PreCookReport(SdlResourceId rawResourceId)
	: m_rawResourceId(rawResourceId)
	, m_baseLocalToWorld(nullptr)
	, m_baseWorldToLocal(nullptr)
	, m_isCookable(true)
{
	PH_ASSERT_NE(rawResourceId, EMPTY_SDL_RESOURCE_ID);
}

PreCookReport& PreCookReport::cookable()
{
	m_isCookable = true;
	return *this;
}

PreCookReport& PreCookReport::uncookable()
{
	m_isCookable = false;
	return *this;
}

PreCookReport& PreCookReport::setBaseTransforms(
	const math::Transform* const localToWorld,
	const math::Transform* const worldToLocal)
{
	m_baseLocalToWorld = localToWorld;
	m_baseWorldToLocal = worldToLocal;
	return *this;
}

bool PreCookReport::isCookable() const
{
	return m_isCookable;
}

const math::Transform* PreCookReport::getBaseLocalToWorld() const
{
	return m_baseLocalToWorld;
}

const math::Transform* PreCookReport::getBaseWorldToLocal() const
{
	return m_baseWorldToLocal;
}

}// end namespace ph
