#pragma once

#include "DataIO/SDL/SdlResourceId.h"

namespace ph
{

class ResourceCookReport final
{
public:
	explicit ResourceCookReport(SdlResourceId rawResourceId);

	void setIsCookable(bool isCookable);
	bool isCookable() const;

private:
	SdlResourceId m_rawResourceId;
	bool          m_isCookable;
};

}// end namespace ph
