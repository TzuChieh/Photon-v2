#pragma once

#include <new>

namespace ph::editor
{

/*! @brief General exception thrown on bad GHI allocation.
*/
class GHIBadAllocation : public std::bad_alloc
{
public:
	const char* what() const noexcept override
	{
		return "Bad GHI allocation.";
	}
};

class GHIOutOfHostMemory : public GHIBadAllocation
{
public:
	const char* what() const noexcept override
	{
		return "Out of host memory.";
	}
};

class GHIOutOfDeviceMemory : public GHIBadAllocation
{
public:
	const char* what() const noexcept override
	{
		return "Out of device memory.";
	}
};

}// end namespace ph::editor
