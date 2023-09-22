#pragma once

#include <Utility/IUninstantiable.h>
#include <Common/primitive_type.h>

#include <string>

namespace ph::editor
{

class IDGenerator : private IUninstantiable
{
public:
	/*! @brief A global number that increases on each call.
	The best option for usages that require unique ID within the program's lifetime.
	Never treat this as UUID.
	@note Thread safe.
	*/
	static uint64 nextCount();

	/*! @brief Similar to `nextCount()`, with a time point associated to it.
	This variant offers better chance that the ID is unique across a program's lifetime.
	The uniqueness of the ID can fail if:
	1. The program generates new IDs in a tight loop (more IDs than the number of time units between
	two consecutive program startup);
	2. User adjusts system time. In the worst case this method degenerates to `nextCount()`.
	Never treat this as UUID.
	@note Thread safe.
	*/
	static uint64 nextTimestampedCount();

	/*! @brief A random number that can be part of a wider ID format.
	The generated number can duplicate though the chance is quite low.
	Never treat this as UUID.
	@note Thread safe.
	*/
	static uint64 nextRandomNumber();

	static std::string toString(uint64 id, int base = 16);
	static std::string toString(uint64 id1, uint64 id2, int base = 16);
};

}// end namespace ph::editor
