#pragma once

#include "Actor/AModel.h"

#include <string>

namespace ph
{

class APhantomModel : public AModel
{
public:
	APhantomModel();
	APhantomModel(
		const std::shared_ptr<Geometry>& geometry,
		const std::shared_ptr<Material>& material);
	APhantomModel(const APhantomModel& other);

	CookedUnit cook(CookingContext& ctx) override;
	CookOrder getCookOrder() const override;

	APhantomModel& operator = (APhantomModel rhs);

	friend void swap(APhantomModel& first, APhantomModel& second);

private:
	std::string m_phantomName;
};

// In-header Implementations:

inline CookOrder APhantomModel::getCookOrder() const
{
	return CookOrder(ECookPriority::HIGH);
}

}// end namespace ph
