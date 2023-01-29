#pragma once

#include "Actor/PhysicalActor.h"

#include <string>

namespace ph
{

class ATransformedInstance : public PhysicalActor
{
public:
	CookedUnit cook(CookingContext& ctx, const PreCookReport& report) override;

private:
	std::string m_phantomName;
};

}// end namespace ph
