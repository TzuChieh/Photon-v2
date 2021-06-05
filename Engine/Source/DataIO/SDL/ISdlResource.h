#pragma once

#include "DataIO/SDL/ETypeCategory.h"

namespace ph
{

class ISdlResource
{
public:
	virtual ~ISdlResource() = default;

	virtual ETypeCategory getCategory() const = 0;
};

}// end namespace ph
