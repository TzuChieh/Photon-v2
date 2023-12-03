#pragma once

#include "Actor/AModel.h"

#include <string>

namespace ph
{

class APhantomModel : public AModel
{
public:
	TransientVisualElement cook(const CookingContext& ctx, const PreCookReport& report) const override;
	CookOrder getCookOrder() const override;

private:
	std::string m_phantomName;
};

}// end namespace ph
