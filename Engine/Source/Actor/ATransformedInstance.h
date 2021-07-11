#pragma once

#include "Actor/PhysicalActor.h"

#include <string>

namespace ph
{

class ATransformedInstance : public PhysicalActor
{
public:
	ATransformedInstance();
	ATransformedInstance(const ATransformedInstance& other);

	CookedUnit cook(ActorCookingContext& ctx) override;

	ATransformedInstance& operator = (ATransformedInstance rhs);

	friend void swap(ATransformedInstance& first, ATransformedInstance& second);

private:
	std::string m_phantomName;
};

}// end namespace ph
