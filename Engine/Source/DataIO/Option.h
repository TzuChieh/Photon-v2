#pragma once

#include "DataIO/SDL/ISdlResource.h"

namespace ph
{

class Option : public ISdlResource
{
public:
	inline Option() = default;
	virtual inline ~Option() = default;

	ETypeCategory getCategory() const override;
};

// In-header Implementations:

inline ETypeCategory Option::getCategory() const
{
	return ETypeCategory::REF_OPTION;
}

}// end namespace ph
