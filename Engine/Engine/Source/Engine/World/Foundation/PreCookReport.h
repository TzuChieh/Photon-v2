#pragma once

#include "Engine/SDL/SdlResourceId.h"

#include <Common/primitive_type.h>

namespace ph { class Transform; }

namespace ph
{

/*! @brief Actor-local data passed from `Actor::preCook()` to `Actor::cook()`.
*/
class PreCookReport final
{
public:
	explicit PreCookReport(SdlResourceId rawResourceId);

	PreCookReport& markAsCookable();
	PreCookReport& markAsUncookable();

	/*! @brief Store dependency-free base transforms for the matching `Actor::cook()`.
	*/
	PreCookReport& setBaseTransforms(
		const Transform* localToWorld, 
		const Transform* worldToLocal);

	bool isCookable() const;
	const Transform* getBaseLocalToWorld() const;
	const Transform* getBaseWorldToLocal() const;

private:
	SdlResourceId m_rawResourceId;
	const Transform* m_baseLocalToWorld;
	const Transform* m_baseWorldToLocal;
	uint32 m_isCookable : 1;
};

}// end namespace ph
