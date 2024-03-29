#pragma once

#include "Actor/PhysicalActor.h"

#include <string>

namespace ph
{

class ATransformedInstance : public PhysicalActor
{
public:
	TransientVisualElement cook(const CookingContext& ctx, const PreCookReport& report) const override;

private:
	std::string m_phantomName;
};

}// end namespace ph
