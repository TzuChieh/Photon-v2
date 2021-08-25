#pragma once

#include "DataIO/SDL/ISdlResource.h"
#include "DataIO/SDL/SdlBaseResource.h"

namespace ph
{

class SdlBaseResource : public ISdlResource
{
protected:
	SdlBaseResource();

public:
	ETypeCategory getCategory() const override = 0;

	SdlResourceId getId() const override;

private:
	SdlResourceId m_resourceId;
};

// In-header Implementations:

inline SdlBaseResource::SdlBaseResource() :

	ISdlResource(),

{

}

}// end namespace ph
