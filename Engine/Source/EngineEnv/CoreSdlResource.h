#pragma once

#include "DataIO/SDL/SdlResourceBase.h"

namespace ph { class CoreCookingContext; }
namespace ph { class CoreCookedUnit; }

namespace ph
{

/*! @brief Core SDL resource abstraction.
This class does not provide static/dynamic category information. The work is left
for derived classes. See SdlResourceBase for more implementation requirements.
*/
class CoreSdlResource : public SdlResourceBase
{
public:
	ESdlTypeCategory getDynamicCategory() const override = 0;

	virtual void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) = 0;
};

}// end namespace ph
