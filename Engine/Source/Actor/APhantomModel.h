#pragma once

#include "Actor/AModel.h"

#include <string>

namespace ph
{

class APhantomModel : public AModel
{
public:
	CookedUnit cook(CookingContext& ctx, const PreCookReport& report) override;
	CookOrder getCookOrder() const override;

private:
	std::string m_phantomName;
};

}// end namespace ph
