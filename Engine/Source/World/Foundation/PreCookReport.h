#pragma once

#include "SDL/SdlResourceId.h"
#include "Common/primitive_type.h"

namespace ph::math { class Transform; }

namespace ph
{

class PreCookReport final
{
public:
	explicit PreCookReport(SdlResourceId rawResourceId);

	PreCookReport& markAsCookable();
	PreCookReport& markAsUncookable();

	PreCookReport& setBaseTransforms(
		const math::Transform* localToWorld, 
		const math::Transform* worldToLocal);

	bool isCookable() const;
	const math::Transform* getBaseLocalToWorld() const;
	const math::Transform* getBaseWorldToLocal() const;

private:
	SdlResourceId m_rawResourceId;
	const math::Transform* m_baseLocalToWorld;
	const math::Transform* m_baseWorldToLocal;
	uint32 m_isCookable : 1;
};

}// end namespace ph
