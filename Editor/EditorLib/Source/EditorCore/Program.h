#pragma once

#include <Utility/IUninstantiable.h>

namespace ph::editor
{

class Program final : private IUninstantiable
{
	friend int application_entry_point(int argc, char* argv[]);

private:
	// Called right after program start. Guaranteed to be on main thread.
	static void onProgramStart();

	// Called right before program exit. Guaranteed to be on main thread.
	static void onProgramExit();
};

}// end namespace ph::editor
