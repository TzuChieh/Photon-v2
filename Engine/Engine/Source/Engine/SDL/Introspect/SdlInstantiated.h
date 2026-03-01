#pragma once

#include "Engine/SDL/TSdlAnyInstance.h"

#include <memory>

namespace ph
{

struct SdlInstantiated
{
	/*! Actual instance. */
	SdlNonConstInstance data;

	/*! Allocation for this instance. May be null if this is a shallow reference. */
	std::shared_ptr<void> allocation;

	/*!
	Checks if `*this` stores a non-null instance.
	*/
	operator bool() const
	{
		return static_cast<bool>(allocation);
	}
};

}// end namespace ph
