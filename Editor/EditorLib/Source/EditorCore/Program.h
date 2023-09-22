#pragma once

#include <Utility/IUninstantiable.h>
#include <Common/primitive_type.h>

namespace ph::editor
{

class Program final : private IUninstantiable
{
public:
	/*!
	@return System time of the program when it started.
	*/
	static uint64 getStartTime();

private:
	static uint64 startTime;

// Methods for the entry point to call only.
private:
	friend int application_entry_point(int argc, char* argv[]);

	// Called right after program start. Guaranteed to be on main thread.
	static void programStart();

	// Called right before program exit. Guaranteed to be on main thread.
	static void programExit();
};

}// end namespace ph::editor
