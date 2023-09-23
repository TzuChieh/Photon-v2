#pragma once

#include <new>

namespace ph::editor::ghi
{

/*! @brief General exception thrown on bad GHI allocation.
*/
class BadAllocation : public std::bad_alloc
{
public:
	const char* what() const noexcept override
	{
		return "Bad GHI allocation.";
	}
};

class OutOfHostMemory : public BadAllocation
{
public:
	const char* what() const noexcept override
	{
		return "Out of host memory.";
	}
};

class OutOfDeviceMemory : public BadAllocation
{
public:
	const char* what() const noexcept override
	{
		return "Out of device memory.";
	}
};

}// end namespace ph::editor::ghi
