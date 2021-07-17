#pragma once

#include "DataIO/SDL/ISdlResource.h"

namespace ph { class CoreCookingContext; }
namespace ph { class CoreCookedUnit; }

namespace ph
{

class ICoreSdlResource : public ISdlResource
{
public:
	ETypeCategory getCategory() const override = 0;

	virtual void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) = 0;
};

}// end namespace ph
